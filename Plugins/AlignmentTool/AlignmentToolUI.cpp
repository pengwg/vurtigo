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
#include "AlignmentToolUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"
#include "rtApplication.h"

#include <QList>
#include <vtkMath.h>

AlignmentToolUI::AlignmentToolUI() {
  setupUi(this);

 // create target and entry point "buffers"  
  m_pointTarget = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Target Point");
  m_pointEntry  = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Entry Point");
  m_pointAiming  = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Aiming Point");
  
 // create target (initial) point
  rtBasic3DPointData p;
  p.setPoint(0, 0, 0);
  p.setPointSize(5);
  p.getProperty()->SetOpacity(0.5);
  p.setColor(1, 0, 0); // red
  rt3DPointBufferDataObject *pointTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  pointTarget->lock();
  pointTarget->addPoint(p);
  pointTarget->Modified();
  pointTarget->unlock();
//  pointTarget->setVisible3D(true);
  
 // create entry (initial) point
  p.setPoint(100, 100, 100);
  p.setPointSize(5);
  p.getProperty()->SetOpacity(0.5);
  p.setColor(1, 1, 0); // yellow
  rt3DPointBufferDataObject *pointEntry = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));
  pointEntry->lock();
  pointEntry->addPoint(p);
  pointEntry->Modified();
  pointEntry->unlock();
//  pointEntry->setVisible3D(true);

// create aiming point
  p.setPoint(200, 200, 200);
  p.setPointSize(5);
  p.getProperty()->SetOpacity(0); // invisible for now
  p.setColor(0, 1, 0); // green
  rt3DPointBufferDataObject *pointAiming = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointAiming));
  pointAiming->lock();
  pointAiming->addPoint(p);
  pointAiming->Modified();
  pointAiming ->unlock();

 // initial distance from insertion point to aiming plane
  aimingOffsetDoubleSpinBox->setValue(50);

 // initial depth is undefined
  depthEdit->setText(""); 

  populateLists();
  connectSignals();
}

AlignmentToolUI::~AlignmentToolUI() {
}

void AlignmentToolUI::connectSignals() {
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(objectAdded(int)) );
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );

  connect( aimingOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(aimingOffsetChanged(double)) );

  rt3DPointBufferDataObject *pointTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  rt3DPointBufferDataObject *pointEntry = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));

  connect(pointEntry, SIGNAL(objectChanged(int)), this, SLOT(pointMoved()));
  connect(pointTarget, SIGNAL(objectChanged(int)), this, SLOT(pointMoved()));

  connect( alignButton, SIGNAL(pressed()), this, SLOT(update())); // xxxxxxxxxxxx
}


void AlignmentToolUI::populateLists() {
  QList<int> planeObjs;
  rt2DSliceDataObject* slice=NULL;

  planeObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_2DObject);

  aimingPlaneComboBox->clear();
  monitoringPlane1ComboBox->clear();
  monitoringPlane2ComboBox->clear();

  for (int ix1=0; ix1<planeObjs.size(); ix1++) {
    slice = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeObjs[ix1]));
    if (slice) {
      m_planeObjectMap.insert(ix1, slice);
      
      aimingPlaneComboBox->insertItem(ix1, QString::number(planeObjs[ix1])+slice->getObjName());
      monitoringPlane1ComboBox->insertItem(ix1, QString::number(planeObjs[ix1])+slice->getObjName());
      monitoringPlane2ComboBox->insertItem(ix1, QString::number(planeObjs[ix1])+slice->getObjName());
    }
  }

}

void AlignmentToolUI::objectAdded(int objID) {
  rtDataObject *data=NULL;
  rt2DSliceDataObject *slice=NULL;

  data = rtBaseHandle::instance().getObjectWithID(objID);
  if (!data) return;

  if ( data->getObjectType() == rtConstants::OT_2DObject ) {
    slice = static_cast<rt2DSliceDataObject*>(data);
    if (!slice) return;
    m_planeObjectMap.insert(m_planeObjectMap.count(), slice);

    aimingPlaneComboBox->insertItem(aimingPlaneComboBox->count(), QString::number(objID)+slice->getObjName());
    monitoringPlane1ComboBox->insertItem(monitoringPlane1ComboBox->count(), QString::number(objID)+slice->getObjName());
    monitoringPlane2ComboBox->insertItem(monitoringPlane2ComboBox->count(), QString::number(objID)+slice->getObjName());
  }

}

//! The object manager has removed an object. It could be of a type we are interesed in.
void AlignmentToolUI::objectRemoved(int objID) {
 // save previous selection indices
  int aimingPlanePos      = aimingPlaneComboBox->currentIndex();
  int monitoringPlane1Pos = monitoringPlane1ComboBox->currentIndex();
  int monitoringPlane2Pos = monitoringPlane2ComboBox->currentIndex();

 // update lists
  populateLists();

 // restore previous selection indices (though this might lead to an offset by one, depending on which was deleted)
  if ( aimingPlanePos >= 0 && aimingPlanePos < aimingPlaneComboBox->count() ) 
    aimingPlaneComboBox->setCurrentIndex(aimingPlanePos);

  if ( monitoringPlane1Pos >= 0 && monitoringPlane1Pos < monitoringPlane1ComboBox->count() ) 
    monitoringPlane1ComboBox->setCurrentIndex(monitoringPlane1Pos);
    
  if ( monitoringPlane2Pos >= 0 && monitoringPlane2Pos < monitoringPlane2ComboBox->count() ) 
    monitoringPlane2ComboBox->setCurrentIndex(monitoringPlane2Pos);    
}

void AlignmentToolUI::pointMoved()
  {
    update();
  }

void AlignmentToolUI::update() {
 // get pointers to point buffer objects (target and entry)
  rt3DPointBufferDataObject *pointBufTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  rt3DPointBufferDataObject *pointBufEntry  = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));
  rt3DPointBufferDataObject *pointBufAiming = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointAiming));
  
 // make sure everything exists
  if (!pointBufTarget || !pointBufEntry || !pointBufAiming)
    return;
  
 // get points
  rtBasic3DPointData pointTarget = *pointBufTarget->getPointAtIndex(0);
  rtBasic3DPointData pointEntry  = *pointBufEntry->getPointAtIndex(0);

// get point positions
 double entryPoint[3], targetPoint[3];
 pointEntry.getPoint(entryPoint);
 pointTarget.getPoint(targetPoint);

 // update "insertion depth" display
  double insertionDepth = rtBasic3DPointData::findDistance(pointTarget, pointEntry);
  depthEdit->setText(QString::number(insertionDepth, 'f', 3)); 

 // get indices to planes
  int aimingPlanePos = aimingPlaneComboBox->currentIndex();
  int monitoringPlane1Pos = monitoringPlane1ComboBox->currentIndex();
  int monitoringPlane2Pos = monitoringPlane2ComboBox->currentIndex();
  
 // get pointers to slice objects (aiming and monitoring)
  rt2DSliceDataObject* sliceAiming      = m_planeObjectMap.value(aimingPlanePos);
  rt2DSliceDataObject* sliceMonitoring1 = m_planeObjectMap.value(monitoringPlane1Pos);
  rt2DSliceDataObject* sliceMonitoring2 = m_planeObjectMap.value(monitoringPlane2Pos);

 // get aiming plane offset distance
  double aimingPlaneOffset = aimingOffsetDoubleSpinBox->value();
  
 // get unit vector in direction from entry point to target point
  double aimingVector[3];
  aimingVector[0] = (targetPoint[0] - entryPoint[0]) / insertionDepth;
  aimingVector[1] = (targetPoint[1] - entryPoint[1]) / insertionDepth;
  aimingVector[2] = (targetPoint[2] - entryPoint[2]) / insertionDepth;
  
 // get "aiming point" by backing off aimingPlaneOffset from the insertion point in the direction of the aimingVector
  double aimingPoint[3];
  aimingPoint[0] = entryPoint[0] - aimingPlaneOffset*aimingVector[0];
  aimingPoint[1] = entryPoint[1] - aimingPlaneOffset*aimingVector[1];
  aimingPoint[2] = entryPoint[2] - aimingPlaneOffset*aimingVector[2];
  
 // adjust "aiming point"
  rtBasic3DPointData *p_pointAiming = pointBufAiming->getPointAtIndex(0);
  p_pointAiming->setPoint(aimingPoint);
  p_pointAiming->getProperty()->SetOpacity(0.5); // make visible
  pointBufAiming->Modified();
  
 // find up vector for aiming plane
  double aimingUp[3];
  aimingUp[0] = 0; aimingUp[1] = 0; aimingUp[2] = 0;
  if ((fabs(aimingVector[0]) > fabs(aimingVector[1])) && (fabs(aimingVector[0]) > fabs(aimingVector[2])))
   // aimed along x
    aimingUp[1] = 1; // "up" vector is in y (A/P) direction
  else if ((fabs(aimingVector[1]) > fabs(aimingVector[0])) && (fabs(aimingVector[1]) > fabs(aimingVector[2])))
   // aimed along y
    aimingUp[0] = 1; // "up" vector is in x (L/R) direction
  else
   // aimed along z
    aimingUp[0] = 1; // "up" vector is in y (A/P) direction
    
 // project aiming up vector onto aiming plane
  double V_dot_N = vtkMath::Dot(aimingUp, aimingVector);
  aimingUp[0] = aimingUp[0] - V_dot_N*aimingVector[0];
  aimingUp[1] = aimingUp[1] - V_dot_N*aimingVector[1];
  aimingUp[2] = aimingUp[2] - V_dot_N*aimingVector[2];
  
 // set aiming plane
  if (sliceAiming)
    {
      sliceAiming->setPlaneCenter(aimingPoint, true);
      sliceAiming->setPlaneNormal(aimingVector, true);
      sliceAiming->setPlaneUp(aimingUp, true);
    }
  
 // first monitoring plane should contain the vector and the z axis
  double perp1[3];
  perp1[0] = 0;  perp1[1] = 0; perp1[2] = 1;
  double norm1[3];
  vtkMath::Cross(aimingVector, perp1, norm1);
  double up1[3];
  vtkMath::Cross(aimingVector, norm1, up1);
  
 // if they're parallel (ambigious), then use the insertion vector and the y axis
  if (vtkMath::Norm(norm1) == 0) 
    {
      perp1[0] = 0;  perp1[1] = 1; perp1[2] = 0;
      vtkMath::Cross(aimingVector, perp1, norm1);
    }
    
 // second monitoring plane should contain the vector and the normal to the first plane
  double perp2[3];
  perp2[0] = norm1[0]; perp2[1] = norm1[1]; perp2[2] = norm1[2];
  double norm2[3];
  vtkMath::Cross(aimingVector, perp2, norm2);
  double up2[3];
  vtkMath::Cross(aimingVector, norm2, up2);
  
 // if they're parallel, then use the insertion vector and the x axis
  if (vtkMath::Norm(norm2) == 0) 
    {
      perp2[0] = 1;  perp2[1] = 0; perp2[2] = 0;
      vtkMath::Cross(aimingVector, perp2, norm2);
    }  
  
 // select center point for monitoring planes (halfway between entry and target point)
  double monitoringCenterPoint[3];
  monitoringCenterPoint[0] = (targetPoint[0] + entryPoint[0]) / 2;
  monitoringCenterPoint[1] = (targetPoint[1] + entryPoint[1]) / 2;
  monitoringCenterPoint[2] = (targetPoint[2] + entryPoint[2]) / 2;
    
 // set monitoring planes
 if (sliceMonitoring1 && (vtkMath::Norm(norm1) != 0))
   {
     sliceMonitoring1->setPlaneCenter(monitoringCenterPoint, true);
     sliceMonitoring1->setPlaneNormal(norm1, true);
     sliceMonitoring1->setPlaneUp(up1, true);
   }

 if (sliceMonitoring2 && (vtkMath::Norm(norm2) != 0))
   {
     sliceMonitoring2->setPlaneCenter(monitoringCenterPoint, true);
     sliceMonitoring2->setPlaneNormal(norm2, true);
     sliceMonitoring2->setPlaneUp(up2, true);
   }
  
}

void AlignmentToolUI::aimingOffsetChanged(double offset) {
  update();
}
