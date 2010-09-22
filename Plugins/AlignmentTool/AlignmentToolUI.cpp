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
  
 // create target (initial) point
  rt3DPointBufferDataObject::SimplePoint p;
  p.px = 0;
  p.py = 0;
  p.pz = 0;
  p.pSize = 5;
  p.pProp->SetOpacity(0.5);
  p.pProp->SetColor(1, 0, 0); // red
  rt3DPointBufferDataObject *pointTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  pointTarget->lock();
  pointTarget->addPoint(p);
  pointTarget->Modified();
  pointTarget->unlock();
//  pointTarget->setVisible3D(true);
  
 // create entry (initial) point
  p.px = 100;
  p.py = 100;
  p.pz = 100;
  p.pSize = 5;
  p.pProp->SetOpacity(0.5);
  p.pProp->SetColor(1, 1, 0); // yellow
  rt3DPointBufferDataObject *pointEntry = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));
  pointEntry->lock();
  pointEntry->addPoint(p);
  pointEntry->Modified();
  pointEntry->unlock();
//  pointEntry->setVisible3D(true);

 // initial distance from insertion point to aiming plane
  aimingOffsetDoubleSpinBox->setValue(5);

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
  
 // make sure everything exists
  if (!pointBufTarget || !pointBufEntry)
    return;
  
 // get points
  rt3DPointBufferDataObject::SimplePoint pointTarget = *pointBufTarget->getPoint(0);
  rt3DPointBufferDataObject::SimplePoint pointEntry  = *pointBufEntry->getPoint(0);
  
 // update "insertion depth" display
  double insertionDepth = findDistance(pointTarget, pointEntry);
  depthEdit->setText(QString::number(insertionDepth, 'f', 3)); 

 // get indices to planes
  int aimingPlanePos = aimingPlaneComboBox->currentIndex();
  int monitoringPlane1Pos = monitoringPlane1ComboBox->currentIndex();
  int monitoringPlane2Pos = monitoringPlane2ComboBox->currentIndex();
  
 // get pointers to slice objects (aiming and monitoring)
  rt2DSliceDataObject* sliceAiming      = m_planeObjectMap.value(aimingPlanePos);
  rt2DSliceDataObject* sliceMonitoring1 = m_planeObjectMap.value(monitoringPlane1Pos);
  rt2DSliceDataObject* sliceMonitoring2 = m_planeObjectMap.value(monitoringPlane2Pos);

 // make sure everything exists
  if (!sliceAiming || !sliceMonitoring1 || !sliceMonitoring2)
    return;
  
 // get aiming plane offset distance
  double aimingPlaneOffset = aimingOffsetDoubleSpinBox->value();
  
 // get unit vector in direction from entry point to target point
  double aimingVector[3];
  aimingVector[0] = (pointTarget.px - pointEntry.px) / insertionDepth;
  aimingVector[1] = (pointTarget.py - pointEntry.py) / insertionDepth;
  aimingVector[2] = (pointTarget.pz - pointEntry.pz) / insertionDepth;
  
 // get "aiming point" by backing off aimingPlaneOffset from the insertion point in the direction of the aimingVector
  double aimingPoint[3];
  aimingPoint[0] = pointEntry.px - aimingPlaneOffset*aimingVector[0];
  aimingPoint[1] = pointEntry.py - aimingPlaneOffset*aimingVector[1];
  aimingPoint[2] = pointEntry.pz - aimingPlaneOffset*aimingVector[2];
  
 // set aiming plane
  sliceAiming->setPlaneCenter(aimingPoint, true);
  sliceAiming->setPlaneNormal(aimingVector, true);
  
 // find normals for monitoring planes
  double perp1[3];
  perp1[0] = 0;  perp1[1] = 1; perp1[2] = 0;
  double norm1[3];
  vtkMath::Cross(aimingVector, perp1, norm1);
  
  double perp2[3];
  perp2[0] = 0;  perp2[1] = 0; perp2[2] = 1;
  double norm2[3];
  vtkMath::Cross(aimingVector, perp2, norm2);
  
 // set monitoring planes
 double entryPoint[3];
 entryPoint[0] = pointEntry.px; 
 entryPoint[1] = pointEntry.py; 
 entryPoint[2] = pointEntry.pz; 
 sliceMonitoring1->setPlaneCenter(entryPoint, true);
 sliceMonitoring2->setPlaneCenter(entryPoint, true);

 sliceMonitoring1->setPlaneNormal(norm1, true);
 sliceMonitoring2->setPlaneNormal(norm2, true);
  
}

void AlignmentToolUI::aimingOffsetChanged(double offset) {
  update();
}

#define SQR(x) ((x)*(x))

double AlignmentToolUI::findDistance(rt3DPointBufferDataObject::SimplePoint p1, rt3DPointBufferDataObject::SimplePoint p2)
  {
    return sqrt(SQR((p1.px-p2.px)) + SQR((p1.py-p2.py)) + SQR((p1.pz-p2.pz)));
  }
