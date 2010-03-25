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
rtEPDataObject::rtEPDataObject() : m_currentPhase(-1),  m_objTransform(0), m_inPlaneInterval(1.0), m_crossPlaneInterval(1.0) {
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
  Modified();
  return true;
}

bool rtEPDataObject::setTriggerDelay(int phase, int trigger) {
  if(phase < 0 || trigger < 0) return false;

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);
  m_phaseDataList[phase].triggerDelay = trigger;
  Modified();
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
  Modified();
  return true;
}

bool rtEPDataObject::setCurrPhase(int phase) {
  if(phase < 0) return false;
  if(!m_phaseDataList.contains(phase)) return false;

  m_currentPhase = phase;
  Modified();
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
  dat.pointDataUpdate = true;

  for (int ix1=0; ix1<3; ix1++) {
    dat.sliceSpline[ix1].clear();
    dat.posSpline[ix1].clear();
  }

  dat.meshData = vtkPolyData::New();
  dat.meshDataUpdate = true;

  m_phaseDataList.insert(phaseNum, dat);

  if(m_currentPhase < 0) m_currentPhase = phaseNum;

  Modified();
}

void rtEPDataObject::deletePhase(int phaseNum) {
  PhaseData dat = m_phaseDataList.take(phaseNum);

  dat.triggerDelay = 0;
  dat.pointList.clear();
  if(dat.pointData) {
    dat.pointData->Delete();
    dat.pointData = NULL;
  }

  dat.pointDataUpdate = false; // no need for updates

  cleanupSliceSpline(&dat);
  cleanupPositionSpline(&dat);

  if(dat.meshData) {
    dat.meshData->Delete();
    dat.meshData = NULL;
  }
  dat.meshDataUpdate = false; // no need for updates
  Modified();
}


//! Send the info to the GUI
void rtEPDataObject::update() {

}

bool rtEPDataObject::addPoint(int phase, int slice, rtEPDataObject::EPPoint pt) {
  if(phase < 0 || slice < 0) return false;

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);

  m_phaseDataList[phase].pointList.insert(slice, pt);
  m_phaseDataList[phase].pointDataUpdate = true;
  m_phaseDataList[phase].meshDataUpdate = true;
  Modified();
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

void rtEPDataObject::cleanupSliceSpline(PhaseData* data) {
  if (!data) return;

  QList<vtkKochanekSpline*> tempList;
  for (int coord = 0; coord<3; coord++) {
    tempList =  data->sliceSpline[coord].values();
    while (!tempList.empty()) {
      vtkKochanekSpline* spline = tempList.takeFirst();
      if(spline) spline->Delete();
    }
    data->sliceSpline[coord].clear();
  }
}

void rtEPDataObject::cleanupPositionSpline(PhaseData* data) {
  if (!data) return;

  QList<vtkKochanekSpline*> tempList;
  for (int coord = 0; coord<3; coord++) {
    tempList =  data->posSpline[coord].values();
    while (!tempList.empty()) {
      vtkKochanekSpline* spline = tempList.takeFirst();
      if(spline) spline->Delete();
    }
    data->posSpline[coord].clear();
  }
}

void rtEPDataObject::updatePointData() {

  if (phaseExists(m_currentPhase)) {
    if (m_phaseDataList[m_currentPhase].pointDataUpdate) {
      vtkPoints* pts = vtkPoints::New();
      vtkCellArray* cells = vtkCellArray::New();
      QList<EPPoint> tempPT;
      vtkIdType pID;

      QList<int> slices = m_phaseDataList.value(m_currentPhase).pointList.uniqueKeys();

      for (int ix1=0; ix1<slices.size(); ix1++) {
        tempPT = m_phaseDataList.value(m_currentPhase).pointList.values(slices[ix1]);
        for (int ix2=0; ix2<tempPT.size(); ix2++) {
          pID = pts->InsertNextPoint(tempPT.at(ix2).x, tempPT.at(ix2).y, tempPT.at(ix2).z);
          cells->InsertNextCell(1, &pID);
        }
      }

      m_phaseDataList[m_currentPhase].pointData->SetPoints(pts);
      m_phaseDataList[m_currentPhase].pointData->SetVerts(cells);
      m_phaseDataList[m_currentPhase].pointDataUpdate = false;

      pts->Delete();
      cells->Delete();
    }
  }
}

void rtEPDataObject::updateMeshData() {
  if (phaseExists(m_currentPhase)) {
    if (m_phaseDataList[m_currentPhase].meshDataUpdate) {
      QList<EPPoint> tempPT, tempPT2;
      QList<int> slices = m_phaseDataList.value(m_currentPhase).pointList.uniqueKeys();
      vtkKochanekSpline *tempSpline[3];
      int maxSlice = 0;
      int maxPos = 0;

      // Populate the slice splines first.
      for (int ix1=0; ix1<slices.size(); ix1++) {
        tempPT = m_phaseDataList[m_currentPhase].pointList.values(slices[ix1]);

        // Determine the maximum slice.
        if(slices[ix1] > maxSlice) maxSlice = slices[ix1];

        for (int coord=0; coord<3; coord++) {
          if (m_phaseDataList[m_currentPhase].sliceSpline[coord].contains(slices[ix1])) {
            tempSpline[coord] = m_phaseDataList[m_currentPhase].sliceSpline[coord].value(slices[ix1]);
            tempSpline[coord]->RemoveAllPoints();
          } else {
            tempSpline[coord] = vtkKochanekSpline::New();
            m_phaseDataList[m_currentPhase].sliceSpline[coord].insert(slices[ix1], tempSpline[coord]);
          }
        }

        for (int ix2=0; ix2<tempPT.size(); ix2++) {
          tempSpline[0]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).x);
          tempSpline[1]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).y);
          tempSpline[2]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).z);

          // Determine the maximum position.
          if (tempPT.at(ix2).loc > maxPos) maxPos = tempPT.at(ix2).loc;
        }
      }

      // Cleaup the old position spline.
      cleanupPositionSpline(&(m_phaseDataList[m_currentPhase]));

      // Now create the position splines based on the slice splines.
      for (double pos=0; pos<=maxPos; pos+=m_inPlaneInterval) {

        for (int coord=0; coord<3; coord++) {
          tempSpline[coord] = vtkKochanekSpline::New();
          m_phaseDataList[m_currentPhase].posSpline[coord].insert(pos, tempSpline[coord]);
        }

        for (int ix1=0; ix1<slices.size(); ix1++) {
          tempSpline[0]->AddPoint(ix1, m_phaseDataList[m_currentPhase].sliceSpline[0].value(slices[ix1])->Evaluate(pos) );
          tempSpline[1]->AddPoint(ix1, m_phaseDataList[m_currentPhase].sliceSpline[1].value(slices[ix1])->Evaluate(pos) );
          tempSpline[2]->AddPoint(ix1, m_phaseDataList[m_currentPhase].sliceSpline[2].value(slices[ix1])->Evaluate(pos) );
        }
      }


      vtkPoints* pts = vtkPoints::New();
      vtkCellArray* cells = vtkCellArray::New();
      vtkIdType* pID = new vtkIdType[4];

      double xyzCoords[3];
      for (double height=0.0; height<=maxSlice-m_crossPlaneInterval; height+=m_crossPlaneInterval) {
        for (double pos=0.0; pos<=maxPos-m_inPlaneInterval; pos+=m_inPlaneInterval) {
          xyzCoords[0] = m_phaseDataList[m_currentPhase].posSpline[0].value(pos)->Evaluate(height);
          xyzCoords[1] = m_phaseDataList[m_currentPhase].posSpline[1].value(pos)->Evaluate(height);
          xyzCoords[2] = m_phaseDataList[m_currentPhase].posSpline[2].value(pos)->Evaluate(height);
          pID[0] = pts->InsertNextPoint(xyzCoords);

          xyzCoords[0] = m_phaseDataList[m_currentPhase].posSpline[0].value(pos)->Evaluate(height+m_crossPlaneInterval);
          xyzCoords[1] = m_phaseDataList[m_currentPhase].posSpline[1].value(pos)->Evaluate(height+m_crossPlaneInterval);
          xyzCoords[2] = m_phaseDataList[m_currentPhase].posSpline[2].value(pos)->Evaluate(height+m_crossPlaneInterval);
          pID[1] = pts->InsertNextPoint(xyzCoords);

          xyzCoords[0] = m_phaseDataList[m_currentPhase].posSpline[0].value(pos+m_inPlaneInterval)->Evaluate(height+m_crossPlaneInterval);
          xyzCoords[1] = m_phaseDataList[m_currentPhase].posSpline[1].value(pos+m_inPlaneInterval)->Evaluate(height+m_crossPlaneInterval);
          xyzCoords[2] = m_phaseDataList[m_currentPhase].posSpline[2].value(pos+m_inPlaneInterval)->Evaluate(height+m_crossPlaneInterval);
          pID[2] = pts->InsertNextPoint(xyzCoords);

          xyzCoords[0] = m_phaseDataList[m_currentPhase].posSpline[0].value(pos+m_inPlaneInterval)->Evaluate(height);
          xyzCoords[1] = m_phaseDataList[m_currentPhase].posSpline[1].value(pos+m_inPlaneInterval)->Evaluate(height);
          xyzCoords[2] = m_phaseDataList[m_currentPhase].posSpline[2].value(pos+m_inPlaneInterval)->Evaluate(height);
          pID[3] = pts->InsertNextPoint(xyzCoords);

          cells->InsertNextCell(4, pID);
        }
      }

      // Finally, create the mesh
      m_phaseDataList[m_currentPhase].meshData->SetPoints(pts);
      m_phaseDataList[m_currentPhase].meshData->SetPolys(cells);

      delete[] pID;
      pts->Delete();
      cells->Delete();

      m_phaseDataList[m_currentPhase].meshDataUpdate = false;
    }
  }
}
