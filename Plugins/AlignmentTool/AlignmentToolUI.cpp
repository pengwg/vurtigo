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
#include "rtRenderObject.h"

#include <QList>
#include <vtkMath.h>

AlignmentToolUI::AlignmentToolUI() {
  setupUi(this);
  
  m_fAutoUpdate = 0;

 // create target and entry point "buffers"  
  m_pointTarget = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Target Point");
  m_pointEntry  = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Entry Point");
  m_pointAiming  = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Aiming Point");
  
 // create target (initial) point
  rtBasic3DPointData p;
  p.setPoint(0, 0, 0);
  p.setPointSize(1);
  p.getProperty()->SetOpacity(0.5);
  p.setColor(1, 0, 0); // red
  rt3DPointBufferDataObject *pointTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  pointTarget->lock();
  pointTarget->addPoint(p);
  pointTarget->Modified();
  pointTarget->unlock();
//  pointTarget->setVisible3D(true);
  
 // create entry (initial) point
  p.setPoint(10, 10, 10);
  p.setPointSize(1);
  p.getProperty()->SetOpacity(0.5);
  p.setColor(1, 1, 0); // yellow
  rt3DPointBufferDataObject *pointEntry = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));
  pointEntry->lock();
  pointEntry->addPoint(p);
  pointEntry->Modified();
  pointEntry->unlock();
//  pointEntry->setVisible3D(true);

// create aiming point
  p.setPoint(20, 20, 20);
  p.setPointSize(1);
  p.getProperty()->SetOpacity(0); // invisible for now
  p.setColor(0, 1, 0); // green
  rt3DPointBufferDataObject *pointAiming = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointAiming));
  pointAiming->lock();
  pointAiming->addPoint(p);
  pointAiming->Modified();
  pointAiming ->unlock();

 // initial distance from insertion point to aiming plane
  aimingOffsetDoubleSpinBox->setValue(50);

 // initial distance from target point to monitoring plane 
  monitoringOffsetDoubleSpinBox->setValue(0);

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
  connect( monitoringOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(monitoringOffsetChanged(double)) );

  rt3DPointBufferDataObject *pointTarget = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointTarget));
  rt3DPointBufferDataObject *pointEntry = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pointEntry));

  connect(pointEntry, SIGNAL(objectChanged(int)), this, SLOT(pointMoved()));
  connect(pointTarget, SIGNAL(objectChanged(int)), this, SLOT(pointMoved()));
  
  connect(aimingPlaneComboBox,      SIGNAL(currentIndexChanged(int)), this, SLOT(planeIndexChanged(int)) );
  connect(trajPlane1ComboBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(planeIndexChanged(int)) );
  connect(trajPlane2ComboBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(planeIndexChanged(int)) );
  connect(monitoringPlaneComboBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(planeIndexChanged(int)) );
  
  connect( updatePlanesCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePlanesChanged(bool)) );

  connect( aimingPlaneVisibleCheckBox,     SIGNAL(toggled(bool)), this, SLOT(planeVisibilityCheckBoxChanged(bool)) );
  connect( trajPlane1VisibleCheckBox,      SIGNAL(toggled(bool)), this, SLOT(planeVisibilityCheckBoxChanged(bool)) );
  connect( trajPlane2VisibleCheckBox,      SIGNAL(toggled(bool)), this, SLOT(planeVisibilityCheckBoxChanged(bool)) );
  connect( monitoringPlaneVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(planeVisibilityCheckBoxChanged(bool)) );
}


int AlignmentToolUI::populateLists() {
  QList<int> planeObjs;
  rt2DSliceDataObject* slice=NULL;

  planeObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_2DObject);

  aimingPlaneComboBox->clear();
  trajPlane1ComboBox->clear();
  trajPlane2ComboBox->clear();
  monitoringPlaneComboBox->clear();

  for (int ix1=0; ix1<planeObjs.size(); ix1++) {
    slice = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeObjs[ix1]));
    if (slice) {
      m_planeObjectMap.insert(ix1, slice);
      
      QString num = QString::number(planeObjs[ix1])+slice->getObjName();
      
      aimingPlaneComboBox->insertItem(ix1, num);
      trajPlane1ComboBox->insertItem(ix1, num);
      trajPlane2ComboBox->insertItem(ix1, num);
      monitoringPlaneComboBox->insertItem(ix1, num);
    }
  }
  
  return planeObjs.size();
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
    
    QString num = QString::number(objID)+slice->getObjName();

    aimingPlaneComboBox->insertItem(aimingPlaneComboBox->count(),           num);
    trajPlane1ComboBox->insertItem(trajPlane1ComboBox->count(),             num);
    trajPlane2ComboBox->insertItem(trajPlane2ComboBox->count(),             num);
    monitoringPlaneComboBox->insertItem(monitoringPlaneComboBox->count(),   num);
  }
  
 // force initial plane selections if we not set
  int aimingPlaneIndex     = aimingPlaneComboBox->currentIndex();
  int trajPlane1Index     = trajPlane1ComboBox->currentIndex();
  int trajPlane2Index      = trajPlane2ComboBox->currentIndex();
  int monitoringPlaneIndex = monitoringPlaneComboBox->currentIndex();
  if (
      (aimingPlaneComboBox->count() == 4) &&
      (aimingPlaneIndex == 0) && 
      (trajPlane1Index == 0)  && 
      (trajPlane2Index == 0)  && 
      (monitoringPlaneIndex == 0)
     )
    {
      aimingPlaneComboBox->setCurrentIndex(0); 
      trajPlane1ComboBox->setCurrentIndex(1); 
      trajPlane2ComboBox->setCurrentIndex(2); 
      monitoringPlaneComboBox->setCurrentIndex(3); 
    }

}

//! The object manager has removed an object. It could be of a type we are interesed in.
void AlignmentToolUI::objectRemoved(int objID) {
 // save previous selection indices
  int aimingPlanePos      = aimingPlaneComboBox->currentIndex();
  int trajPlane1Pos       = trajPlane1ComboBox->currentIndex();
  int trajPlane2Pos       = trajPlane2ComboBox->currentIndex();
  int monitoringPlanePos  = monitoringPlaneComboBox->currentIndex();

 // update lists
  populateLists();

 // restore previous selection indices (though this might lead to an offset by one, depending on which was deleted)
  if ( aimingPlanePos >= 0 && aimingPlanePos < aimingPlaneComboBox->count() ) 
    aimingPlaneComboBox->setCurrentIndex(aimingPlanePos);

  if ( trajPlane1Pos >= 0 && trajPlane1Pos < trajPlane1ComboBox->count() ) 
    trajPlane1ComboBox->setCurrentIndex(trajPlane1Pos);

  if ( trajPlane2Pos >= 0 && trajPlane2Pos < trajPlane2ComboBox->count() ) 
    trajPlane2ComboBox->setCurrentIndex(trajPlane2Pos);    

  if ( monitoringPlanePos >= 0 && monitoringPlanePos < monitoringPlaneComboBox->count() ) 
    monitoringPlaneComboBox->setCurrentIndex(monitoringPlanePos);    

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
  
 // give up if target point equals entry point
  if (insertionDepth == 0)
    return;

 // get indices to planes
  int aimingPlanePos     = aimingPlaneComboBox->currentIndex();
  int trajPlane1Pos      = trajPlane1ComboBox->currentIndex();
  int trajPlane2Pos      = trajPlane2ComboBox->currentIndex();
  int monitoringPlanePos = monitoringPlaneComboBox->currentIndex();
  
 // get pointers to slice objects (aiming and monitoring)
  rt2DSliceDataObject* sliceAiming     = m_planeObjectMap.value(aimingPlanePos);
  rt2DSliceDataObject* sliceTraj1      = m_planeObjectMap.value(trajPlane1Pos);
  rt2DSliceDataObject* sliceTraj2      = m_planeObjectMap.value(trajPlane2Pos);
  rt2DSliceDataObject* sliceMonitoring = m_planeObjectMap.value(monitoringPlanePos);

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
  
 // skip out now if the "update planes" flag isn't set
  if (!m_fAutoUpdate)
    return;

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
  
 // first trajectory plane should contain the vector and the z axis
  double perp1[3];
  perp1[0] = 0;  perp1[1] = 0; perp1[2] = 1;
  double norm1[3];
  vtkMath::Cross(aimingVector, perp1, norm1);
  double up1[3];
  vtkMath::Cross(aimingVector, norm1, up1);
  
 // if they're parallel (ambigious), then instead make it contain the insertion vector and the y axis
  if (vtkMath::Norm(norm1) == 0) 
    {
      perp1[0] = 0;  perp1[1] = 1; perp1[2] = 0;
      vtkMath::Cross(aimingVector, perp1, norm1);
    }
    
 // second trajectory plane should contain the aiming vector and the normal to the first plane
  double perp2[3];
  perp2[0] = norm1[0]; perp2[1] = norm1[1]; perp2[2] = norm1[2];
  double norm2[3];
  vtkMath::Cross(aimingVector, perp2, norm2);
  double up2[3];
  vtkMath::Cross(aimingVector, norm2, up2);
  
 // if they're parallel, then instead use make it contain the insertion vector and the x axis
  if (vtkMath::Norm(norm2) == 0) 
    {
      perp2[0] = 1;  perp2[1] = 0; perp2[2] = 0;
      vtkMath::Cross(aimingVector, perp2, norm2);
    }  
  
 // select center point for monitoring planes (halfway between entry and target point)
  double trajCenterPoint[3];
  trajCenterPoint[0] = (targetPoint[0] + entryPoint[0]) / 2;
  trajCenterPoint[1] = (targetPoint[1] + entryPoint[1]) / 2;
  trajCenterPoint[2] = (targetPoint[2] + entryPoint[2]) / 2;
    
 // set trajectory-aligned planes
 if (sliceTraj1 && (vtkMath::Norm(norm1) != 0) && (sliceTraj1 != sliceAiming))
   {
     sliceTraj1->setPlaneCenter(trajCenterPoint, true);
     sliceTraj1->setPlaneNormal(norm1, true);
     sliceTraj1->setPlaneUp(up1, true);
   }

 if (sliceTraj2 && (vtkMath::Norm(norm2) != 0)  && (sliceTraj1 != sliceAiming) && (sliceTraj2 != sliceTraj1))
   {
     sliceTraj2->setPlaneCenter(trajCenterPoint, true);
     sliceTraj2->setPlaneNormal(norm2, true);
     sliceTraj2->setPlaneUp(up2, true);
   }
 
 // get monitoring plane offset distance
  double monitoringPlaneOffset = monitoringOffsetDoubleSpinBox->value();
  
 // get "monitoring point" by backing off monitoringPlaneOffset from the target point in the direction of the aimingVector (plus is deeper)
  double monitoringPoint[3];
  monitoringPoint[0] = targetPoint[0] + monitoringPlaneOffset*aimingVector[0];
  monitoringPoint[1] = targetPoint[1] + monitoringPlaneOffset*aimingVector[1];
  monitoringPoint[2] = targetPoint[2] + monitoringPlaneOffset*aimingVector[2]; 

 // set monitoring plane
 if (sliceMonitoring && (vtkMath::Norm(aimingVector) != 0) && (sliceMonitoring != sliceAiming) && (sliceMonitoring != sliceTraj1) && (sliceMonitoring != sliceTraj2))
   {
     sliceMonitoring->setPlaneCenter(monitoringPoint, true);
     sliceMonitoring->setPlaneNormal(aimingVector, true);
     sliceMonitoring->setPlaneUp(aimingUp, true);
   }
  
}

void AlignmentToolUI::aimingOffsetChanged(double offset) {
  update();
}

void AlignmentToolUI::monitoringOffsetChanged(double offset) {
  update();
}


void AlignmentToolUI::updatePlanesChanged(bool value)
  {
    m_fAutoUpdate = value;
    
    if (m_fAutoUpdate)
      update();
  }

void AlignmentToolUI::planeIndexChanged(int index)
 // we don't know which combo box triggered this, but it doesn't matter - the behavior is the same
  {
    update();
  }

void AlignmentToolUI::planeVisibilityCheckBoxChanged(bool dummy)
  {
   // get indices to planes
    int indexAimingPlane     = aimingPlaneComboBox->currentIndex();
    int indexTrajPlane1      = trajPlane1ComboBox->currentIndex();
    int indexTrajPlane2      = trajPlane2ComboBox->currentIndex();
    int indexMonitoringPlane = monitoringPlaneComboBox->currentIndex();

   // get pointers to slice objects 
    rt2DSliceDataObject* sliceAiming     = m_planeObjectMap.value(indexAimingPlane);
    rt2DSliceDataObject* sliceTraj1      = m_planeObjectMap.value(indexTrajPlane1);
    rt2DSliceDataObject* sliceTraj2      = m_planeObjectMap.value(indexTrajPlane2);
    rt2DSliceDataObject* sliceMonitoring = m_planeObjectMap.value(indexMonitoringPlane);
    
   // get object identifiers
    int idAimingPlane          = sliceAiming     ? sliceAiming->getId()     : -1;
    int idTrajPlane1           = sliceTraj1      ? sliceTraj1->getId()      : -1;
    int idTrajPlane2           = sliceTraj2      ? sliceTraj2->getId()      : -1;
    int idMonitoringPlane      = sliceMonitoring ? sliceMonitoring->getId() : -1;

    if (sliceAiming)
      rtBaseHandle::instance().setObjectVisible3D(idAimingPlane, aimingPlaneVisibleCheckBox->isChecked());
      
    if (sliceTraj1)
      rtBaseHandle::instance().setObjectVisible3D(idTrajPlane1, trajPlane1VisibleCheckBox->isChecked());

    if (sliceTraj2)
      rtBaseHandle::instance().setObjectVisible3D(idTrajPlane2, trajPlane2VisibleCheckBox->isChecked());

    if (sliceMonitoring)
      rtBaseHandle::instance().setObjectVisible3D(idMonitoringPlane, monitoringPlaneVisibleCheckBox->isChecked());

/*    
   // get render objects
    rtRenderObject *renderAimingPlane     = rtBaseHandle::instance().getRenderObjectWithID(idAimingPlane);
    rtRenderObject *renderTrajPlane1      = rtBaseHandle::instance().getRenderObjectWithID(idTrajPlane1);
    rtRenderObject *renderTrajPlane2      = rtBaseHandle::instance().getRenderObjectWithID(idTrajPlane2);
    rtRenderObject *renderMonitoringPlane = rtBaseHandle::instance().getRenderObjectWithID(idMonitoringPlane);
    
   // set visibility
    if (renderAimingPlane)
      {
        renderAimingPlane->setVisible3D(aimingPlaneVisibleCheckBox->isChecked());
        sliceAiming->Modified();
        renderAimingPlane->updateTreeItem();
      }

    if (renderTrajPlane1)
      {
        renderTrajPlane1->setVisible3D(trajPlane1VisibleCheckBox->isChecked());
        sliceTraj1->Modified();
        renderTrajPlane1->updateTreeItem();
      }

    if (renderTrajPlane2)
      {
        renderTrajPlane2->setVisible3D(trajPlane2VisibleCheckBox->isChecked());
        sliceTraj2->Modified();
        renderTrajPlane2->updateTreeItem();
      }

    if (renderMonitoringPlane)
      {
        renderMonitoringPlane->setVisible3D(monitoringPlaneVisibleCheckBox->isChecked());
        sliceMonitoring->Modified();
        renderMonitoringPlane->updateTreeItem();
      }
*/      
  }
