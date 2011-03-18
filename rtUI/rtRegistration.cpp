/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "rtRegistration.h"
#include "vtkCellPicker.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkImageReslice.h"
#include "vtkPoints.h"

#include "rtApplication.h"
#include "rtObjectManager.h"
#include "objTypes.h"
#include "rtMessage.h"
#include "rtRenderObject.h"
#include "rtDataObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rt3dVolumeRenderObject.h"
#include "rt3dVolumeDataObject.h"
#include "customQVTKWidget.h"
#include "rtBasic3DPointData.h"
#include <QTableWidgetItem>
#include <QTableWidget>
#include "vtkPointPicker.h"
#include "vtkPropPicker.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "rtMainWindow.h"

rtRegistration::rtRegistration(QWidget *parent, Qt::WindowFlags flags)
{
  setupUi(this);

  connect(registerButton, SIGNAL(pressed()), this, SLOT(registerButtonPressed()));

  connect (sourceRadio, SIGNAL(clicked()), this, SLOT(sourceRadioChosen()));
  connect (targetRadio, SIGNAL(clicked()), this, SLOT(targetRadioChosen()));


  connect (refreshButton, SIGNAL(pressed()), this, SLOT(setupAllCombos()));

  connect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setupSourceTable()));
  connect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTargetTable()));

  connect (volSource, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));
  connect (volTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));

  connect (newPointsButton, SIGNAL(pressed()), this, SLOT(addNewPoints()));

  connect (syncSTBox, SIGNAL(toggled(bool)), this ,SLOT(syncToggled(bool)));

  connect (colorMatchButton, SIGNAL(pressed()), this, SLOT(colorMatchPressed()));

  connect(this, SIGNAL(rejected()), this, SLOT(placementOff()));

  setOneTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  setTwoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  m_colorList = QColor::colorNames();
  m_moved = false;
  // set the source active
  m_activeSet = 0;
  sourceFrame->setLineWidth(5);
  targetFrame->setLineWidth(5);
  sourceFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  targetFrame->setFrameStyle(QFrame::NoFrame);

  registerBox->addItem("Rigid");
  registerBox->addItem("Similarity");
  registerBox->addItem("Affine");
  registerBox->addItem("Thin Plate Spline");

  setupAllCombos();



}

//! Destructor
rtRegistration::~rtRegistration() {
}

void rtRegistration::sourceRadioChosen()
{
    m_activeSet = 0;
    sourceFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    targetFrame->setFrameStyle(QFrame::NoFrame);
    // update synchs
    syncToggled(syncSTBox->isChecked());

}

void rtRegistration::targetRadioChosen()
{
    m_activeSet = 1;
    targetFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    sourceFrame->setFrameStyle(QFrame::NoFrame);
    syncToggled(syncSTBox->isChecked());
}

void rtRegistration::placementOn()
{
    // Connect mouse actions
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();
    connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(addActivePoint(QMouseEvent*)));
    connect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoved(QMouseEvent *)));
    //connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(doubleClicked(QMouseEvent*)));

}

void rtRegistration::placementOff()
{
    // Disconnect mouse actions
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();
    disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(addActivePoint(QMouseEvent*)));
    disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoved(QMouseEvent*)));
    //disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(doubleClicked(QMouseEvent*)));

}

void rtRegistration::volumeChanged()
{
    syncToggled(syncSTBox->isChecked());
}

void rtRegistration::mouseMoved(QMouseEvent *event)
{
    if (event->buttons() == Qt::RightButton)
        m_moved = true;
}


void rtRegistration::doubleClicked(QMouseEvent *event)
{

}

void rtRegistration::syncToggled(bool flag)
{
    // no volume loaded
    if (volSource->currentIndex() < 0) return;
    if (volTarget->currentIndex() < 0) return;

    rt3DVolumeRenderObject *sObj;
    rt3DVolumeRenderObject *tObj;
    for (int ix1=0; ix1<m_volumes.count(); ix1++)
    {
        sObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(ix1)));
        // may need to be a little more precise here!
        if (sObj) sObj->removeAllSync();
    }

    if (flag && (sObj != tObj))
    {
        sObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex())));
        tObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volTarget->currentIndex())));
        sObj->addSyncObject(tObj);
        tObj->addSyncObject(sObj);

    }

}

//! The user has clicked on register so the changes will be made.
void rtRegistration::registerButtonPressed() {

    rt3DPointBufferDataObject *source = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
    rt3DPointBufferDataObject *target = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
    rt3DVolumeDataObject *volume = static_cast<rt3DVolumeDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex()))->getDataObject());

    if (source->getPointListSize() != target->getPointListSize())
        return;


    vtkLandmarkTransform *Ltransform = vtkLandmarkTransform::New();
    vtkThinPlateSplineTransform *TPStransform = vtkThinPlateSplineTransform::New();
    vtkPoints *sources = vtkPoints::New();
    vtkPoints *targets = vtkPoints::New();

    for (int ix1; ix1<source->getPointListSize(); ix1++)
    {
        double p[3];
        source->getPointAtIndex(ix1)->getPoint(p);
        sources->InsertNextPoint(p);
        target->getPointAtIndex(ix1)->getPoint(p);
        targets->InsertNextPoint(p);
    }

    switch(registerBox->currentIndex())
    {
    case 0:
        Ltransform->SetModeToRigidBody();
        Ltransform->SetSourceLandmarks(sources);
        Ltransform->SetTargetLandmarks(targets);
        break;
    case 1:
        Ltransform->SetModeToSimilarity();
        Ltransform->SetSourceLandmarks(sources);
        Ltransform->SetTargetLandmarks(targets);
        break;
    case 2:
        Ltransform->SetModeToAffine();
        Ltransform->SetSourceLandmarks(sources);
        Ltransform->SetTargetLandmarks(targets);
        break;
    case 3:
        TPStransform->SetSourceLandmarks(sources);
        TPStransform->SetTargetLandmarks(targets);
        TPStransform->SetBasisToR();
    }



    vtkImageReslice *reslice = vtkImageReslice::New();
    reslice->SetInput(volume->getImageData());
    reslice->SetOutputExtentToDefault();
    reslice->SetOutputSpacingToDefault();
    reslice->SetOutputOriginToDefault();
    if (registerBox->currentIndex() < 3)
        reslice->SetResliceTransform(Ltransform);
    else
        reslice->SetResliceTransform(TPStransform);

    rtRenderObject* temp;
    temp = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_3DObject,QString(volume->getObjName() + "_R_" + QString::number(source->getId()) + "_to_" + QString::number(target->getId())));

    Ltransform->Delete();
    TPStransform->Delete();
    sources->Delete();
    targets->Delete();

}

void rtRegistration::addActivePoint(QMouseEvent *event)
{
    // if we moved the mouse, don't put a point
    if (m_moved)
    {
        // reset the mouse moved flag
        m_moved = false;
        return;
    }

    if (event->button() == Qt::RightButton)
    {
        double pos[3];
        int res;
        QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget()->size();
        int X = event->x();
        int Y = winSize.height()-event->y();
        vtkCellPicker *pick = vtkCellPicker::New();
        res = pick->Pick(X,Y,0,rtApplication::instance().getMainWinHandle()->getRenderer());
        pick->GetPickPosition(pos);
        rt3DPointBufferDataObject *dObj;
        rtBasic3DPointData newPoint;
        if (res)
        {

            if (m_activeSet == 0)
            {
                dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
                newPoint.setPoint(pos);
                // if the other table has more points, match the colors

                if (setOneTable->rowCount() < setTwoTable->rowCount())
                    m_color = (setTwoTable->item(setOneTable->rowCount(),3))->backgroundColor();
                else
                    // get a random color
                    m_color = QColor(m_colorList.at(qrand() % m_colorList.size()));

                //m_color = QColor(m_colorList.at(setOneTable->rowCount()+1));
                newPoint.getProperty()->SetColor(m_color.red() / 255.0,m_color.green() / 255.0,m_color.blue() / 255.0);
                dObj->lock();
                dObj->addPoint(newPoint);
                dObj->Modified();
                dObj->unlock();
                setupSourceTable();

            }
            else if (m_activeSet == 1)
            {
                rt3DVolumeRenderObject *rObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volTarget->currentIndex())));
                if (!rObj->hasProp(pick->GetViewProp())) return;
                dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
                // set position and color of new point
                newPoint.setPoint(pos);
                // if the other table has more points, match the colors

                if (setOneTable->rowCount() > setTwoTable->rowCount())
                    m_color = (setOneTable->item(setTwoTable->rowCount(),3))->backgroundColor();
                else
                    // get a random color
                    m_color = QColor(m_colorList.at(qrand() % m_colorList.size()));

                //m_color = QColor(m_colorList.at(setTwoTable->rowCount()+1));
                newPoint.getProperty()->SetColor(m_color.red() / 255.0,m_color.green() / 255.0,m_color.blue() / 255.0);
                dObj->lock();
                dObj->addPoint(newPoint);
                dObj->Modified();
                dObj->unlock();
                setupTargetTable();

            }

        }
    }


}


void rtRegistration::addNewPoints()
{
    QString name = "Placement " + newPointsName->text();

    rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_3DPointBuffer,name);
    setupPointCombos();
}

void rtRegistration::setupPointCombos()
{
    // need to turn off these signals or they will cause assert failures
    disconnect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setupSourceTable()));
    disconnect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTargetTable()));
    setSource->clear();
    setTarget->clear();
    setOneTable->clearContents();
    setTwoTable->clearContents();
    m_points = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_3DPointBuffer);
    if (m_points.empty()) return;
    for (int ix1=0; ix1<m_points.count(); ix1++)
    {
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(ix1))->getDataObject());
        QString objName = dObj->getObjName();
        if (!dObj) return;
        setSource->addItem(objName + " " + QString::number(m_points.at(ix1)));
        setTarget->addItem(objName + " " + QString::number(m_points.at(ix1)));

    }
    setupSourceTable();
    setupTargetTable();
    // reconnect
    connect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setupSourceTable()));
    connect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTargetTable()));


}

void rtRegistration::setupVolumeCombos()
{
    volSource->clear();
    volTarget->clear();

    m_volumes = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_3DObject);
    if (m_volumes.empty()) return;
    for (int ix1=0; ix1<m_volumes.count(); ix1++)
    {
        rt3DVolumeDataObject *dObj = static_cast<rt3DVolumeDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(ix1))->getDataObject());
        QString objName = dObj->getObjName();
        if (!dObj) return;
        volSource->addItem(objName + " " + QString::number(m_volumes.at(ix1)));
        volTarget->addItem(objName + " " + QString::number(m_volumes.at(ix1)));

    }

}

void rtRegistration::setupSourceTable()
{
    // remove old data
    setOneTable->clearContents();
    int ix1;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
    if (!dObj) return;

    setOneTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        //setOneTable->insertRow(ix1);
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        setOneTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        setOneTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        setOneTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        setOneTable->setItem(ix1,3,newItem);
    }

}

void rtRegistration::setupTargetTable()
{
    // remove old data
    setTwoTable->clearContents();
    int ix1;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
    if (!dObj) return;


    setTwoTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        //setTwoTable->insertRow(ix1);
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        setTwoTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        setTwoTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        setTwoTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        setTwoTable->setItem(ix1,3,newItem);
    }

}

void rtRegistration::setupAllCombos()
{
    setupVolumeCombos();
    setupPointCombos();
}

void rtRegistration::colorMatchPressed()
{
    int min;
    rt3DPointBufferDataObject *sObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
    rt3DPointBufferDataObject *tObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
    if (setOneTable->rowCount() < setTwoTable->rowCount())
        min = setOneTable->rowCount();
    else
        min = setTwoTable->rowCount();

    for (int ix1=0; ix1<min; ix1++)
    {
        m_color = QColor(m_colorList.at(qrand() % m_colorList.size()));
        sObj->getPointAtIndex(ix1)->getProperty()->SetColor(m_color.red() / 255.0,m_color.green() / 255.0,m_color.blue() / 255.0);
        tObj->getPointAtIndex(ix1)->getProperty()->SetColor(m_color.red() / 255.0,m_color.green() / 255.0,m_color.blue() / 255.0);
    }

    sObj->Modified();
    tObj->Modified();

    setupSourceTable();
    setupTargetTable();
}



