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

//! Constructor to the dialog
pointPlacementDialog::pointPlacementDialog(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect (refreshButton, SIGNAL(pressed()), this, SLOT(setupCombo()));

  //connect when index changes on boxes - call setupTables

  connect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));


  connect (newPointsButton, SIGNAL(pressed()), this, SLOT(addNewPoints()));

  connect(this, SIGNAL(rejected()), this, SLOT(placementOff()));

  m_colorList = QColor::colorNames();
  setupCombo();


}

pointPlacementDialog::~pointPlacementDialog() {

}

void pointPlacementDialog::placementOn()
{
    // Connect mouse actions
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();
    connect(renWid, SIGNAL(placeMousePress(QMouseEvent*)), this, SLOT(addPoint(QMouseEvent*)));

}

void pointPlacementDialog::placementOff()
{
    // Disconnect mouse actions
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();
    disconnect(renWid, SIGNAL(placeMousePress(QMouseEvent*)), this, SLOT(addPoint(QMouseEvent*)));
}

void pointPlacementDialog::addPoint(QMouseEvent *event)
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

void pointPlacementDialog::addNewPoints()
{
    QString name = "Placement " + newPointsName->text();

    rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_3DPointBuffer,name);
    setupCombo();
}


void pointPlacementDialog::setupCombo()
{
    // need to turn off these signals or they will cause assert failures
    disconnect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));
    setCombo->clear();
    setTable->clearContents();

    m_points = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_3DPointBuffer);
    if (m_points.empty()) return;   
    for (int ix1=0; ix1<m_points.count(); ix1++)
    {
        rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(ix1))->getDataObject());
        QString objName = dObj->getObjName();
        if (!dObj) return;
        setCombo->addItem(objName + " " + QString::number(m_points.at(ix1)));

    }
    setupTable();
    // reconnect
    connect (setCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setupTable()));


}
void pointPlacementDialog::setupTable()
{
    // remove old data
    setTable->clearContents();
    int ix1;
    rt3DPointBufferDataObject *dObj = static_cast<rt3DPointBufferDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_points.at(setCombo->currentIndex()))->getDataObject());
    if (!dObj) return;
    
    setTable->setRowCount(dObj->getPointListSize());

    for (ix1=0; ix1<dObj->getPointListSize(); ix1++)
    {

        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getX()));
        setTable->setItem(ix1,0,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getY()));
        setTable->setItem(ix1,1,newItem);
        newItem = new QTableWidgetItem(QString::number(dObj->getPointAtIndex(ix1)->getZ()));
        setTable->setItem(ix1,2,newItem);
        newItem = new QTableWidgetItem();
        newItem->setBackgroundColor(dObj->getPointAtIndex(ix1)->getColor());
        setTable->setItem(ix1,3,newItem);
    }

}
