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
#include "rtEPDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

#include <QHash>
#include <QColor>
#include <QString>

#include <vtkPoints.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

//! Constructor
rtEPDataObject::rtEPDataObject() : m_currentPhase(-1),  m_objTransform(0) {
  setObjectType(rtConstants::OT_EPMesh);

  m_phaseDataList.clear();

  m_objTransform = vtkTransform::New();

  setupGUI();
}

//! Destructor
rtEPDataObject::~rtEPDataObject() {
  cleanupGUI();

  QList<int> listKeys = m_phaseDataList.keys();
  for (int ix1=0; ix1<listKeys.size(); ix1++) {
    deletePhase(listKeys.at(ix1));
  }

  if (m_objTransform) {
    m_objTransform->Delete();
    m_objTransform = NULL;
  }
}



bool rtEPDataObject::setTransform(vtkTransform* t) {
  if (!t || !m_objTransform) return false;

  m_objTransform->DeepCopy(t);

  return true;
}

bool rtEPDataObject::setTriggerDelay(int phase, int trigger) {
  if(phase < 0 || trigger < 0) return false;

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);
  m_phaseDataList[phase].triggerDelay = trigger;
  return true;
}

bool rtEPDataObject::setCurrTrigDelay(int trigDelay) {
  if (trigDelay < 0) return false;

  QList<int> listKeys = m_phaseDataList.keys();

  int phase = 0;
  int diff = abs(trigDelay - m_phaseDataList.value(listKeys.value(0)).triggerDelay);
  // Find the closest trigger dealy and set the phase

  for (int ix1=1; ix1<listKeys.size(); ix1++) {
    if (diff > abs(trigDelay - m_phaseDataList.value(listKeys.value(ix1)).triggerDelay)) {
      phase = listKeys.value(ix1);
      diff = abs(trigDelay - m_phaseDataList.value(listKeys.value(ix1)).triggerDelay);
    }
  }
  m_currentPhase = phase;

  return true;
}

bool rtEPDataObject::setCurrPhase(int phase) {
  if(phase < 0) return false;
  if(!m_phaseDataList.contains(phase)) return false;

  m_currentPhase = phase;
  return true;
}


bool rtEPDataObject::phaseExists(int phaseNum) {
  return m_phaseDataList.contains(phaseNum);
}

void rtEPDataObject::createPhase(int phaseNum) {
  if (phaseExists(phaseNum)) return;

  PhaseData dat;

  dat.triggerDelay = 0;
  dat.pointList.clear();
  dat.pointData = vtkPolyData::New();
  dat.pointSet = vtkPoints::New();
  dat.pointDataUpdate = true;
  dat.sliceSplineX.clear();
  dat.sliceSplineY.clear();
  dat.sliceSplineZ.clear();
  dat.posSplineX.clear();
  dat.posSplineY.clear();
  dat.posSplineZ.clear();
  dat.meshData = vtkPolyData::New();
  dat.meshDataUpdate = true;

  m_phaseDataList.insert(phaseNum, dat);
}

void rtEPDataObject::deletePhase(int phaseNum) {
  PhaseData dat = m_phaseDataList.take(phaseNum);

  dat.triggerDelay = 0;
  dat.pointList.clear();
  if(dat.pointData) {
    dat.pointData->Delete();
    dat.pointData = NULL;
  }

  if (dat.pointSet) {
    dat.pointSet->Delete();
    dat.pointSet = NULL;
  }
  dat.pointDataUpdate = false; // no need for updates

  QList<vtkKochanekSpline*> tempList;

  tempList =  dat.sliceSplineX.values();  
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.sliceSplineX.clear();

  tempList =  dat.sliceSplineY.values();
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.sliceSplineY.clear();

  tempList =  dat.sliceSplineZ.values();
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.sliceSplineZ.clear();


  tempList =  dat.posSplineX.values();
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.posSplineX.clear();

  tempList =  dat.posSplineY.values();
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.posSplineY.clear();

  tempList =  dat.posSplineZ.values();
  while (!tempList.empty()) {
    vtkKochanekSpline* spline = tempList.takeFirst();
    if(spline) spline->Delete();
  }
  dat.posSplineZ.clear();


  if(dat.meshData) {
    dat.meshData->Delete();
    dat.meshData = NULL;
  }
  dat.meshDataUpdate = false; // no need for updates
}


//! Send the info to the GUI
void rtEPDataObject::update() {

}

bool rtEPDataObject::addPoint(int phase, int slice, rtEPDataObject::EPPoint pt) {
  if(phase < 0 || slice < 0) return false;

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);

  m_phaseDataList[phase].pointList.insert(slice, pt);
  m_phaseDataList[phase].pointSet->InsertNextPoint(pt.x, pt.y, pt.z);
  m_phaseDataList[phase].pointDataUpdate = true;
  m_phaseDataList[phase].meshDataUpdate = true;
  return true;
}

QList<rtEPDataObject::EPPoint> rtEPDataObject::getPoints(int phase, int slice) {
  return m_phaseDataList.value(phase).pointList.values(slice);
}


vtkPolyData* rtEPDataObject::getPointData() {
  if (phaseExists(m_currentPhase)) {
    updatePointData();
    return m_phaseDataList.value(m_currentPhase).pointData;
  } else {
    return NULL;
  }
}

vtkPolyData* rtEPDataObject::getMeshData() {
  if (phaseExists(m_currentPhase)) {
    updateMeshData();
    return m_phaseDataList.value(m_currentPhase).meshData;
  } else {
    return NULL;
  }
}

bool rtEPDataObject::saveFile(QFile *file) {
  if (!file->open(QIODevice::WriteOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for writing. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  // TODO ***

  file->close();
  return true;
}

bool rtEPDataObject::loadFile(QFile *file) {
  if (!file->open(QIODevice::ReadOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for reading. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  // TODO ***

  Modified();
  return true;
}

////////////////
// Protected
////////////////
//! Set the GUI widgets.
void rtEPDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtEPDataObject::cleanupGUI() {

}

void rtEPDataObject::updatePointData() {
  if (phaseExists(m_currentPhase)) {
    if (m_phaseDataList[m_currentPhase].pointDataUpdate) {
      m_phaseDataList[m_currentPhase].pointData->SetPoints(m_phaseDataList[m_currentPhase].pointSet);
      m_phaseDataList[m_currentPhase].pointDataUpdate = false;
    }
  }
}

void rtEPDataObject::updateMeshData() {
  if (phaseExists(m_currentPhase)) {
    // TODO ***
  }
}
