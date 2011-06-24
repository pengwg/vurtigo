/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
#include <QInputDialog>

rtRegistration::rtRegistration(QWidget *parent, Qt::WindowFlags flags)
{
  setupUi(this);

  connect(registerButton, SIGNAL(pressed()), this, SLOT(registerButtonPressed()));

  connect (sourceRadio, SIGNAL(clicked()), this, SLOT(sourceRadioChosen()));
  connect (targetRadio, SIGNAL(clicked()), this, SLOT(targetRadioChosen()));


  connect (refreshButton, SIGNAL(pressed()), this, SLOT(setupAllCombos()));

  // if an object is added,removed or renamed from Vurtigo. reset the combo boxes just in case it was a 3DPoint or 3D Volume object
  connect(rtApplication::instance().getObjectManager(),SIGNAL(objectCreated(int)),this,SLOT(setupAllCombos()));
  connect(rtApplication::instance().getObjectManager(),SIGNAL(objectRemoved(int)),this,SLOT(setupAllCombos()));
  connect(rtApplication::instance().getObjectManager(),SIGNAL(objectRenamed(int)),this,SLOT(setupAllCombos()));

  connect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceChanged()));
  connect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setTargetChanged()));

  connect (volSource, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));
  connect (volTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));

  connect (newPointsButton, SIGNAL(pressed()), this, SLOT(addNewPoints()));

  connect (syncSTBox, SIGNAL(toggled(bool)), this ,SLOT(syncToggled(bool)));

  connect (colorMatchButton, SIGNAL(pressed()), this, SLOT(colorMatchPressed()));

  connect(this, SIGNAL(rejected()), this, SLOT(placementOff()));

  connect(setOneTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableOneChanged(int,int)));
  connect(setTwoTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableTwoChanged(int,int)));

  connect(numLabels, SIGNAL(pressed()), this, SLOT(numberLabels()));

  m_colorList = QColor::colorNames();
  m_moved = false;
  // set the source active
  m_activeSet = 0;
  m_error = 0;
  m_currVolSource = NULL;
  m_currVolTarget = NULL;
  sourceFrame->setLineWidth(5);
  targetFrame->setLineWidth(5);
  sourceFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  targetFrame->setFrameStyle(QFrame::NoFrame);

  registerBox->addItem("Rigid");
  registerBox->addItem("Similarity");
  registerBox->addItem("Affine");
  registerBox->addItem("Thin Plate Spline");

  regInfo->clear();

  //start with 2 new sets of points
  m_currSetSource = rtApplication::instance().getObjectManager()->addObjectOfType("OT_3DPointBuffer","Placement SOURCE");
  m_currSetTarget = rtApplication::instance().getObjectManager()->addObjectOfType("OT_3DPointBuffer","Placement TARGET");

  setupAllCombos();
}

//! Destructor
rtRegistration::~rtRegistration() {
}

void rtRegistration::sourceRadioChosen()
{
    m_activeSet = 0;
    sourceRadio->setToolTip("Source is active");
    targetRadio->setToolTip("Target is inactive");
    sourceFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    targetFrame->setFrameStyle(QFrame::NoFrame);
    // update synchs
    syncToggled(syncSTBox->isChecked());

}

void rtRegistration::targetRadioChosen()
{
    m_activeSet = 1;
    sourceRadio->setToolTip("Source is inactive");
    targetRadio->setToolTip("Target is active");
    targetFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    sourceFrame->setFrameStyle(QFrame::NoFrame);
    syncToggled(syncSTBox->isChecked());
}

void rtRegistration::placementOn()
{
    // Connect mouse actions
    customQVTKWidget* renWid;
    for (int ix1=0;ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1);
        connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(addActivePoint(QMouseEvent*,int)));
        connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoved(QMouseEvent *,int)));
        //connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(doubleClicked(QMouseEvent*)));
    }

}

void rtRegistration::placementOff()
{
    // Disconnect mouse actions
    customQVTKWidget* renWid;
    for (int ix1=0;ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1);
        disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(addActivePoint(QMouseEvent*,int)));
        disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoved(QMouseEvent*,int)));
        //disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(doubleClicked(QMouseEvent*)));
    }

}

void rtRegistration::volumeChanged()
{
    syncToggled(syncSTBox->isChecked());
    rtRenderObject *rObj;
    if (volSource->currentIndex() < 0) return;
    rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex()));
    m_currVolSource = rObj;
    if (volTarget->currentIndex() < 0) return;
    rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volTarget->currentIndex()));
    m_currVolTarget = rObj;
}

void rtRegistration::mouseMoved(QMouseEvent *event,int window)
{
    if (event->buttons() == Qt::RightButton)
        m_moved = true;
}


void rtRegistration::doubleClicked(QMouseEvent *event,int window)
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
        if (sObj)
            sObj->removeAllSync();
    }

    if (flag)
    {
        sObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex())));
        tObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volTarget->currentIndex())));
        if (sObj != tObj)
        {
            sObj->addSyncObject(tObj);
            tObj->addSyncObject(sObj);
        }

    }

}

// The user has clicked on register so the changes will be made.
void rtRegistration::registerButtonPressed() {
    if (m_points.isEmpty() || m_volumes.isEmpty()) return;
    rtRenderObject *st = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()));
    rtRenderObject *tt = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()));
    rtRenderObject *vol = rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex()));
    if (!st || !tt || !vol) return;

    rt3DPointBufferDataObject *source = static_cast<rt3DPointBufferDataObject*>(st->getDataObject());
    rt3DPointBufferDataObject *target = static_cast<rt3DPointBufferDataObject*>(tt->getDataObject());
    rt3DVolumeDataObject *volume = static_cast<rt3DVolumeDataObject*>(vol->getDataObject());


    if (source->getPointListSize() != target->getPointListSize())
        return;


    vtkLandmarkTransform *Ltransform = vtkLandmarkTransform::New();
    vtkThinPlateSplineTransform *TPStransform = vtkThinPlateSplineTransform::New();
    vtkPoints *sources = vtkPoints::New();
    vtkPoints *targets = vtkPoints::New();
    vtkPoints *sourcesFinal = vtkPoints::New();
    vtkPoints *targetsFinal = vtkPoints::New();
    QString strRegType;

    int min = 0;
    if (source->getPointListSize() < target->getPointListSize())
        min = source->getPointListSize();
    else
        min = target->getPointListSize();

    for (int ix1=0; ix1<min; ix1++)
    {
        double p[3];
        source->getPointAtIndex(ix1)->getPoint(p);
        sources->InsertNextPoint(p);
        target->getPointAtIndex(ix1)->getPoint(p);
        targets->InsertNextPoint(p);
    }
    // transform points into coord system of volume
    volume->getTransform()->TransformPoints(sources,sourcesFinal);
    volume->getTransform()->TransformPoints(targets,targetsFinal);

    switch(registerBox->currentIndex())
    {
    case 0:
        Ltransform->SetModeToRigidBody();
        Ltransform->SetSourceLandmarks(sourcesFinal);
        Ltransform->SetTargetLandmarks(targetsFinal);
        strRegType = "Rigid";
        break;
    case 1:
        Ltransform->SetModeToSimilarity();
        Ltransform->SetSourceLandmarks(sourcesFinal);
        Ltransform->SetTargetLandmarks(targetsFinal);
        strRegType = "Similarity";
        break;
    case 2:
        Ltransform->SetModeToAffine();
        Ltransform->SetSourceLandmarks(sourcesFinal);
        Ltransform->SetTargetLandmarks(targetsFinal);
        strRegType = "Affine";
        break;
    case 3:
        Ltransform->SetModeToAffine();
        Ltransform->SetSourceLandmarks(sourcesFinal);
        Ltransform->SetTargetLandmarks(targetsFinal);
        TPStransform->SetBasisToR();
        strRegType = "TPSpline";
    }

    vtkImageChangeInformation* undoChangeImgCenterInfo = vtkImageChangeInformation::New();
    vtkImageChangeInformation* changeImgCenterInfo = vtkImageChangeInformation::New();
    vtkImageReslice *reslice = vtkImageReslice::New();
    vtkTransform *TPSAffine = vtkTransform::New();

    if (registerBox->currentIndex() > 2)
    {
        // perform an affine transform to line up volumes
        Ltransform->Inverse();
        TPSAffine->SetMatrix(Ltransform->GetMatrix());
        TPSAffine->Concatenate(volume->getTransform());

        // transform target points by the affine, so that source and target fit together within IMAGES!!! coord system
        // MAKE SURE WHEN VOLUME MOVE IS DONE that source and target points are transformed into ORIGINAL volume/image coords!!!!
        vtkPoints *affineTarget = vtkPoints::New();
        Ltransform->TransformPoints(targetsFinal,affineTarget);

        //set the TPS transform
        TPStransform->SetSourceLandmarks(sourcesFinal);
        TPStransform->SetTargetLandmarks(affineTarget);


        //Pass new volume through vtkImageChangeInformation Filter to center image for rotations and translations
        changeImgCenterInfo->SetInput(volume->getImageData());
        changeImgCenterInfo->CenterImageOn();   //Set the origin of the output so that the image coordinate (0,0,0) lies at center of the data set
        changeImgCenterInfo->Update();


        //perform the appropriate image transformation/resampling through vtkImageReslice       
        reslice->SetInput(changeImgCenterInfo->GetOutput());
        reslice->SetOutputExtentToDefault();
        reslice->SetOutputSpacingToDefault();
        reslice->SetOutputOriginToDefault();
        reslice->SetResliceTransform(TPStransform);

        reslice->SetInterpolationModeToLinear();
        reslice->Update();

        //remove the previous origin translation        
        undoChangeImgCenterInfo->SetInput(reslice->GetOutput());
        undoChangeImgCenterInfo->SetOriginTranslation(volume->getImageData()->GetCenter()); //reset the origin
        undoChangeImgCenterInfo->Update();

        //affineSource->Delete();
        affineTarget->Delete();
    }

    rtRenderObject* newObject;
    newObject = rtApplication::instance().getObjectManager()->addObjectOfType("OT_3DObject",QString(volume->getObjName() + ":" + strRegType + "_R_" + QString::number(source->getId()) + "_to_" + QString::number(target->getId())));

    if (newObject)
    {
        rt3DVolumeRenderObject *regRender = static_cast<rt3DVolumeRenderObject*>(newObject);
        rt3DVolumeDataObject* regObj = static_cast<rt3DVolumeDataObject*>(newObject->getDataObject());
        regObj->lock();
        vtkTransform *newTrans = vtkTransform::New();

        if (registerBox->currentIndex() < 3)
        {

            Ltransform->Inverse();
            // add the transform and image data to the new volume
            newTrans->SetMatrix(Ltransform->GetMatrix());
            newTrans->Concatenate(volume->getTransform());
            regObj->copyNewTransform(newTrans);
            regObj->copyNewImageData(volume->getImageData());

            // add a new point object with the transformed points
            vtkPoints *sourceTransformed = vtkPoints::New();
            vtkTransform *volInv = vtkTransform::New();
            volInv->SetMatrix(newTrans->GetMatrix());
            volInv->Inverse();

            volInv->TransformPoints(sourcesFinal,sourceTransformed);



            rt3DPointBufferDataObject *sourceTransPoints = static_cast<rt3DPointBufferDataObject *>(rtApplication::instance().getObjectManager()->addObjectOfType("OT_3DPointBuffer",QString(strRegType + "_R_" + QString::number(source->getId()) + "_to_" + QString::number(target->getId())))->getDataObject());
            if (sourceTransPoints)
            {
                rtBasic3DPointData *newPoint = new rtBasic3DPointData();
                for (int ix1=0; ix1<sourceTransformed->GetNumberOfPoints();ix1++)
                {
                    newPoint->setPoint(sourceTransformed->GetPoint(ix1));
                    QColor c = source->getPointAtIndex(ix1)->getColor();
                    newPoint->getProperty()->SetColor(c.red() / 255.0,c.green() / 255.0,c.blue() / 255.0);
                    sourceTransPoints->lock();
                    sourceTransPoints->addPoint(*newPoint);
                    sourceTransPoints->Modified();
                    // again, would be better to use addCustomPoint from markSet changes when they are done
                    sourceTransPoints->getPointAtIndex(ix1)->setLabel(QString(strRegType + " " + source->getPointAtIndex(ix1)->getLabel()));
                    sourceTransPoints->unlock();
                }
                delete newPoint;
            }

            sourceTransformed->Delete();
            volInv->Delete();

            m_error = calculateError(target, sourceTransPoints);
            if (m_error >= 0)
            {
                QString infoText = QString(volume->getObjName() + ":" + strRegType + "_R_" + QString::number(source->getId()) + "_to_" + QString::number(target->getId()) + " Avg Distance = " + QString::number(m_error) + " mm \n\n");
                regInfo->append(infoText);
            }


        }
        else
        {
            regObj->copyNewTransform(TPSAffine);
            regObj->copyNewImageData(undoChangeImgCenterInfo->GetOutput());            
        }

        regObj->copyTriggerDelayList(volume->getTriggerDelayList());
        regObj->copyDicomCommonData(volume->getDicomCommonData());
        regObj->Modified();
        regRender->copyObject(vol);
        regObj->unlock();
        newTrans->Delete();
    }

    //Cleanup
    sources->Delete();
    targets->Delete();
    sourcesFinal->Delete();
    targetsFinal->Delete();

    Ltransform->Delete();
    TPStransform->Delete();

    changeImgCenterInfo->Delete();
    reslice->Delete();
    undoChangeImgCenterInfo->Delete();
    TPSAffine->Delete();

}

void rtRegistration::addActivePoint(QMouseEvent *event,int window)
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
        QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->size();
        int X = event->x();
        int Y = winSize.height()-event->y();
        vtkCellPicker *pick = vtkCellPicker::New();
        res = pick->Pick(X,Y,0,rtApplication::instance().getMainWinHandle()->getRenderer(window));
        pick->GetPickPosition(pos);
        rt3DPointBufferDataObject *dObj;
        rtBasic3DPointData newPoint;
        if (res)
        {

            if (m_activeSet == 0)
            {
                // can't put  a point on anything other than the chosen object
                if (volSource->currentIndex() < 0) return;
                rt3DVolumeRenderObject *rObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(volSource->currentIndex())));
                if (!rObj->hasProp(pick->GetViewProp())) return;

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
                // give the new point a label (this line should be removed after markset is done so that we use addcustompoint above rather than this, to add the label)
                dObj->getPointAtIndex(dObj->getPointListSize() - 1)->setLabel("S" + QString::number(dObj->getPointListSize()-1));
                setupSourceTable();
                setupTargetTable();

            }
            else if (m_activeSet == 1)
            {
                // can't put  a point on anything other than the chosen object
                if (volTarget->currentIndex() < 0) return;
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
                // give the new point a label (this line should be removed after markset is done so that we use addcustompoint above rather than this, to add the label)
                dObj->getPointAtIndex(dObj->getPointListSize() - 1)->setLabel("T" + QString::number(dObj->getPointListSize()-1));
                setupSourceTable();
                setupTargetTable();

            }

        }
    }


}


void rtRegistration::addNewPoints()
{
    bool ok;
    QString name = QInputDialog::getText(this,"Point Set Name","New Point Set Name: ",QLineEdit::Normal,"Placement",&ok);
    if (!name.isEmpty() && ok)
    {
        rtApplication::instance().getObjectManager()->addObjectOfType("OT_3DPointBuffer",name);
        setupPointCombos();
    }
}

void rtRegistration::setupPointCombos()
{
    // need to turn off these signals or they will cause assert failures
    disconnect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceChanged()));
    disconnect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setTargetChanged()));
    setSource->clear();
    setTarget->clear();
    setOneTable->clearContents();
    setTwoTable->clearContents();
    m_points = rtApplication::instance().getObjectManager()->getObjectsOfType("OT_3DPointBuffer");
    if (m_points.empty()) return;
    int indexS = 0;
    int indexT = 0;
    for (int ix1=0; ix1<m_points.count(); ix1++)
    {
        rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(ix1));
        if (!rObj) return;
        if (rObj == m_currSetSource)
            indexS = ix1;
        if (rObj == m_currSetTarget)
            indexT = ix1;
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
        if (!dObj) return;
        QString objName = dObj->getObjName();
        setSource->addItem(objName + " " + QString::number(m_points.at(ix1)));
        setTarget->addItem(objName + " " + QString::number(m_points.at(ix1)));

    }
    setSource->setCurrentIndex(indexS);
    setTarget->setCurrentIndex(indexT);
    setupSourceTable();
    setupTargetTable();
    // reconnect
    connect (setSource, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceChanged()));
    connect (setTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(setTargetChanged()));


}

void rtRegistration::setupVolumeCombos()
{
    // need to turn these off or they cause problems
    disconnect (volSource, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));
    disconnect (volTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));
    volSource->clear();
    volTarget->clear();

    m_volumes = rtApplication::instance().getObjectManager()->getObjectsOfType("OT_3DObject");

    // sort by studydate
    QMultiMap<QDate, int> map;
    foreach (const int &i, m_volumes)
        map.insert(static_cast<rt3DVolumeDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(i)->getDataObject())->getDicomCommonData()->getStudyDate(), i);
    m_volumes = map.values();

    if (m_volumes.empty()) return;
    int indexS = 0;
    int indexT = 0;
    for (int ix1=0; ix1<m_volumes.count(); ix1++)
    {
        rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_volumes.at(ix1));
        if (!rObj) return;
        if (rObj == m_currVolSource)
            indexS = ix1;
        if (rObj == m_currVolTarget)
            indexT = ix1;
        rt3DVolumeDataObject *dObj = static_cast<rt3DVolumeDataObject*>(rObj->getDataObject());
        if (!dObj) return;
        QString objName = dObj->getObjName();
        volSource->addItem(objName + " " + QString::number(m_volumes.at(ix1)));
        volTarget->addItem(objName + " " + QString::number(m_volumes.at(ix1)));

    }
    volSource->setCurrentIndex(indexS);
    volTarget->setCurrentIndex(indexT);
    volumeChanged();
    // reconnect
    connect (volSource, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));
    connect (volTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(volumeChanged()));

}

void rtRegistration::setupSourceTable()
{
    // turn off signals so they don't get called while we update
    disconnect(setOneTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableOneChanged(int,int)));
    // remove old data
    setOneTable->clearContents();
    int ix1;
    if (setSource->currentIndex() < 0) return;
    rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()));
    if (!rObj) return;
    m_currSetSource = rObj;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
    if (!dObj) return;

    setOneTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setOneTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setOneTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setOneTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setOneTable->setItem(ix1,3,newItem);
        newItem = new QTableWidgetItem(dObj->getPointAtIndex(ix1)->getLabel());
        setOneTable->setItem(ix1,4,newItem);
    }

    connect(setOneTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableOneChanged(int,int)));
}

void rtRegistration::setupTargetTable()
{
    // turn off signals so they don't get called while we update
    disconnect(setTwoTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableTwoChanged(int,int)));
    // remove old data
    setTwoTable->clearContents();
    int ix1;
    if (setTarget->currentIndex() < 0) return;
    rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()));
    if (!rObj) return;
    m_currSetTarget = rObj;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
    if (!dObj) return;


    setTwoTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTwoTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTwoTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTwoTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTwoTable->setItem(ix1,3,newItem);
        newItem = new QTableWidgetItem(dObj->getPointAtIndex(ix1)->getLabel());
        setTwoTable->setItem(ix1,4,newItem);
    }

    connect(setTwoTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableTwoChanged(int,int)));

}

void rtRegistration::setSourceChanged()
{
    rtRenderObject *rObj = NULL;
    if (setSource->currentIndex() >= 0)
        rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()));
    rt3DPointBufferDataObject *dObj = NULL;
    if (rObj)
    {
        dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
        m_currSetSource = rObj;
    }
    if (dObj)
        //connect a signal so if the point changes. we update the list
        connect(dObj, SIGNAL(pointListModifiedSignal()),this,SLOT(setupSourceTable()));
    setupSourceTable();
}

void rtRegistration::setTargetChanged()
{
    rtRenderObject *rObj = NULL;
    if (setTarget->currentIndex() >= 0)
        rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()));
    rt3DPointBufferDataObject *dObj = NULL;
    if (rObj)
    {
        dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
        m_currSetTarget = rObj;
    }
    if (dObj)
        //connect a signal so if the point changes. we update the list
        connect(dObj, SIGNAL(pointListModifiedSignal()),this,SLOT(setupTargetTable()));
    setupTargetTable();
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

void rtRegistration::tableOneChanged(int row,int col)
{
    if (col == 4)
    {
        QTableWidgetItem *item = setOneTable->item(row,col);
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
        if (!dObj) return;

        dObj->getPointAtIndex(row)->setLabel(item->text());
        dObj->pointListModifiedSlot();
        dObj->tableCellChanged(row,5);


        //matching label on other side?
        dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
        if (!dObj) return;
        if (row < dObj->getPointListSize())
        {
            dObj->getPointAtIndex(row)->setLabel(item->text());
            dObj->pointListModifiedSlot();
            dObj->tableCellChanged(row,5);
        }
        if (setTwoTable->item(row,col)) setTwoTable->item(row,col)->setText(item->text());
    }
}

void rtRegistration::tableTwoChanged(int row,int col)
{
    if (col == 4)
    {
        QTableWidgetItem *item = setTwoTable->item(row,col);
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
        if (!dObj) return;

        dObj->getPointAtIndex(row)->setLabel(item->text());
        dObj->pointListModifiedSlot();
        dObj->tableCellChanged(row,5);


        //matching label on other side?
        dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
        if (!dObj) return;
        if (row < dObj->getPointListSize())
        {
            dObj->getPointAtIndex(row)->setLabel(item->text());
            dObj->pointListModifiedSlot();
            dObj->tableCellChanged(row,5);
        }
        if (setOneTable->item(row,col)) setOneTable->item(row,col)->setText(item->text());
    }
}

void rtRegistration::numberLabels()
{
    if (setSource->currentIndex() < 0) return;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setSource->currentIndex()))->getDataObject());
    int ix1;
    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        dObj->getPointAtIndex(ix1)->setLabel(QString::number(ix1));
    }
    dObj->Modified();
    dObj->pointListModifiedSlot();
    dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setTarget->currentIndex()))->getDataObject());
    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {
        dObj->getPointAtIndex(ix1)->setLabel(QString::number(ix1));
    }
    dObj->Modified();
    dObj->pointListModifiedSlot();

    setupSourceTable();
    setupTargetTable();
}

double rtRegistration::calculateError(rt3DPointBufferDataObject *one, rt3DPointBufferDataObject *two)
{
    if (!one || !two) return -1;
    // A list of all squared errors
    //QList<double> Errors;
    double error = 0;
    int min = 0;
    int oneS = one->getPointListSize();
    int twoS = two->getPointListSize();
    min = (oneS < twoS) ? oneS : twoS;
    for (int ix1=0; ix1<min; ix1++)
    {
        /*
         // a different error measure
        double tmp = 0;
        int ax,bx,ay,by,az,bz;
        ax = one->getPointAtIndex(ix1)->getX();
        bx = two->getPointAtIndex(ix1)->getX();
        ay = one->getPointAtIndex(ix1)->getY();
        by = two->getPointAtIndex(ix1)->getY();
        az = one->getPointAtIndex(ix1)->getZ();
        bz = two->getPointAtIndex(ix1)->getZ();
        tmp = tmp + ((one->getPointAtIndex(ix1)->getX() - two->getPointAtIndex(ix1)->getX()) * (one->getPointAtIndex(ix1)->getX() - two->getPointAtIndex(ix1)->getX()));
        tmp = tmp + ((one->getPointAtIndex(ix1)->getY() - two->getPointAtIndex(ix1)->getY()) * (one->getPointAtIndex(ix1)->getY() - two->getPointAtIndex(ix1)->getY()));
        tmp = tmp + ((one->getPointAtIndex(ix1)->getZ() - two->getPointAtIndex(ix1)->getZ()) * (one->getPointAtIndex(ix1)->getZ() - two->getPointAtIndex(ix1)->getZ()));
        //Errors.append(tmp);
        error  = error + tmp;
        */
        error = error + rtBasic3DPointData::findDistance(*(one->getPointAtIndex(ix1)),*(two->getPointAtIndex(ix1)));
    }

    return error / min;

}
