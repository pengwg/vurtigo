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
#include "pointPlacementDialog.h"
#include "vtkCellPicker.h"
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

//! Constructor to the dialog
pointPlacementDialog::pointPlacementDialog(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect (refreshButton, SIGNAL(pressed()), this, SLOT(setupCombo()));

  //connect when index changes on boxes - call setupTables

  connect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));


  connect (newPointsButton, SIGNAL(pressed()), this, SLOT(addNewPoints()));

  connect(this, SIGNAL(rejected()), this, SLOT(placementOff()));

  connect(setTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableChanged(int,int)));

  m_colorList = QColor::colorNames();
  m_moved = false;

  //start with a new set of points
  m_currPoints = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_3DPointBuffer,"Placement");
  setupCombo();


}

pointPlacementDialog::~pointPlacementDialog() {

}

void pointPlacementDialog::placementOn()
{
    // Connect mouse actions
    customQVTKWidget* renWid;
    for (int ix1=0;ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1);
        connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoved(QMouseEvent *,int)));
        connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(addPoint(QMouseEvent*,int)));
    }
}

void pointPlacementDialog::placementOff()
{
    // Disconnect mouse actions
    customQVTKWidget* renWid;
    for (int ix1=0;ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1);
        disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoved(QMouseEvent *,int)));
        disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(addPoint(QMouseEvent*,int)));
    }
}

void pointPlacementDialog::mouseMoved(QMouseEvent *event,int window)
{
    if (event->buttons() == Qt::RightButton)
        m_moved = true;
}

void pointPlacementDialog::addPoint(QMouseEvent *event,int window)
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
        if (res)
        {

            dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setCombo->currentIndex()))->getDataObject());
            rtBasic3DPointData newPoint;
            newPoint.setPoint(pos);

            m_color = QColor(m_colorList.at(qrand() % m_colorList.size()));

            newPoint.getProperty()->SetColor(m_color.red() / 255.0,m_color.green() / 255.0,m_color.blue() / 255.0);
            dObj->lock();
            dObj->addPoint(newPoint);
            dObj->Modified();
            dObj->unlock();


            setupTable();

          }
        pick->Delete();
    }

}

void pointPlacementDialog::addNewPoints()
{
    bool ok;
    QString name = QInputDialog::getText(this,"Point Set Name","New Point Set Name: ",QLineEdit::Normal,"Placement",&ok);
    if (!name.isEmpty() && ok)
    {
        m_currPoints = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_3DPointBuffer,name);
        setupCombo();
    }
}


void pointPlacementDialog::setupCombo()
{
    // need to turn off these signals or they will cause assert failures
    disconnect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));
    setCombo->clear();
    setTable->clearContents();

    m_points = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_3DPointBuffer);
    if (m_points.empty()) return;
    int index = 0;
    for (int ix1=0; ix1<m_points.count(); ix1++)
    {
        rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(ix1));
        if (rObj == m_currPoints)
            index = ix1;
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
        if (!dObj) return;
        QString objName = dObj->getObjName();
        setCombo->addItem(objName + " " + QString::number(m_points.at(ix1)));

    }
    setCombo->setCurrentIndex(index);
    setupTable();
    // reconnect
    connect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));


}
void pointPlacementDialog::setupTable()
{
    // Disconnect the table so that we don't get all the changed events.
    disconnect(setTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableChanged(int,int)));
    // remove old data
    setTable->clearContents();
    int ix1;
    if (setCombo->currentIndex() < 0) return;
    rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setCombo->currentIndex()));
    m_currPoints = rObj;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rObj->getDataObject());
    if (!dObj) return;
    
    setTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {

        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        setTable->setItem(ix1,3,newItem);
        newItem = new QTableWidgetItem(dObj->getPointAtIndex(ix1)->getLabel());
        setTable->setItem(ix1,4,newItem);
    }

    connect(setTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableChanged(int,int)));

    //connect a signal so if the point changes. we update the list
    connect(dObj, SIGNAL(pointListModifiedSignal()),this,SLOT(setupTable()));

}

void pointPlacementDialog::tableChanged(int row,int col)
{
    if (col == 4)
    {
        QTableWidgetItem *item = setTable->item(row,col);
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setCombo->currentIndex()))->getDataObject());
        if (!dObj) return;
        dObj->getPointAtIndex(row)->setLabel(item->text());
        dObj->pointListModifiedSlot();
        dObj->tableCellChanged(row,5);
    }
}
