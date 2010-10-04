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
#include "rtEPInfoObject.h"

#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkMath.h>

rtEPInfoObject::rtEPInfoObject()
 : m_radius(25)
{
  m_defaultColorFunc = vtkColorTransferFunction::New();
  m_defaultColorFunc->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  m_defaultColorFunc->AddRGBPoint(0.5, 0.5, 0.5, 0.5);
  m_defaultColorFunc->AddRGBPoint(1.0, 1.0, 0.0, 0.0);

  m_currentPropertyName = "";

  m_pointPolyData = vtkAppendPolyData::New();
  m_pointPolyData->UserManagedInputsOn();
  m_pointPolyData->RemoveAllInputs();
  cleanupSphereList();
  cleanupSphereScalarList();
}

rtEPInfoObject::~rtEPInfoObject() {
  m_defaultColorFunc->Delete();
  m_pointPolyData->Delete();
  cleanupHash();
  cleanupSphereList();
  cleanupSphereScalarList();
}


void rtEPInfoObject::addInfoPoint(rtNamedInfoPointData p) {
  QList<QString> tagList = p.getTagList();
  QString currName;

  // Add all of the tags for this list
  for (int ix1=0; ix1<tagList.size(); ix1++) {
    currName = tagList[ix1];
    if (m_pointLists.isEmpty()) {
      rtEPPropertyPointList* temp = new rtEPPropertyPointList(currName);
      m_pointLists.insert(currName, temp);
      temp->addPointToList(p);
      m_currentPropertyName = currName;
    } else if(m_pointLists.contains(currName)) {
      m_pointLists.value(currName)->addPointToList(p);
    } else {
      rtEPPropertyPointList* temp = new rtEPPropertyPointList(currName);
      m_pointLists.insert(currName, temp);
      temp->addPointToList(p);
    }
  }
}

bool rtEPInfoObject::getInfoPoint(double x, double y, double z, rtNamedInfoPointData &p, QString propName) {
  if(m_pointLists.contains(propName)) {
    return m_pointLists.value(propName)->getInfoPoint(x, y, z, p);
  } else {
    return false;
  }
}

bool rtEPInfoObject::removeInfoPoint(double x, double y, double z, rtNamedInfoPointData &p, QString propName) {
  if(m_pointLists.contains(propName)) {
    return m_pointLists.value(propName)->removeInfoPoint(x, y, z, p);
  } else {
    return false;
  }
}

void rtEPInfoObject::clearPointList(QString propName) {
  if(m_pointLists.contains(propName)) {
    m_pointLists.value(propName)->clearPointList();
  }
}

void rtEPInfoObject::clearAllPointList() {
  QList<QString> keys;
  keys = m_pointLists.uniqueKeys();

  for (int ix1=0; ix1<keys.size(); ix1++) {
    m_pointLists.value(keys[ix1])->clearPointList();
  }
}

bool rtEPInfoObject::updateScalars(vtkPolyData* data) {
  if (!data) return false;
  if (m_currentPropertyName=="") return false;
  if (!m_pointLists.contains(m_currentPropertyName)) return false;

  // Get the correct list
  rtEPPropertyPointList* ptList = m_pointLists.value(m_currentPropertyName);

  // There must be at least TWO points of info for this to work.
  if (ptList->getNumPoints() < 2) return true;

  // If the max and the min are equal then there is no point in doing this.
  if (ptList->getMaxValue() == ptList->getMinValue()) return true;

  float val;
  vtkIdType numPts;
  double weightSum=0.0;
  double tempPT[3], currLocation[3];
  double currDist;
  double propValueDiff = ptList->getMaxValue()-ptList->getMinValue();

  numPts = data->GetNumberOfPoints();
  vtkFloatArray * scalars = vtkFloatArray::New();

  // Save the transformed points first.
  // Done mainly to trade space for speed.
  double* transPointList = new double[ptList->getNumPoints()*4];
  for (int ix1=0; ix1<ptList->getNumPoints(); ix1++) {
    rtNamedInfoPointData pt = ptList->getPointAt(ix1);
    pt.getPoint(currLocation);
    transPointList[ix1*3] = currLocation[0];
    transPointList[ix1*3+1] = currLocation[1];
    transPointList[ix1*3+2] = currLocation[2];
    transPointList[ix1*3+3] = pt.getValue(m_currentPropertyName);
  }

  for (vtkIdType currID=0; currID<numPts; currID++) {
    weightSum = 1.0;
    val = 0.5;
    data->GetPoint(currID, tempPT);

    for (int ix1=0; ix1<ptList->getNumPoints(); ix1++) {
      currLocation[0] = transPointList[ix1*3];
      currLocation[1] = transPointList[ix1*3+1];
      currLocation[2] = transPointList[ix1*3+2];

      currDist = vtkMath::Distance2BetweenPoints(tempPT, currLocation);
      if (currDist < 0.01) currDist = 0.01;
      if (currDist < m_radius) {
        val += (((double)(transPointList[ix1*3+3] - ptList->getMinValue()))/propValueDiff)*(m_radius/currDist);
        weightSum += (m_radius/currDist);
      }
    }
    val = val/weightSum;
    scalars->InsertTuple(currID, &val);

  }
  data->GetPointData()->SetScalars(scalars);
  scalars->Delete();
  delete[] transPointList;

  return true;
}

vtkPolyData* rtEPInfoObject::getPointPolyData() {
  updatePointPolyData();
  return m_pointPolyData->GetOutput();
}

//////////////////////
// Protected Functions
//////////////////////
void rtEPInfoObject::cleanupHash() {
  if (m_pointLists.isEmpty()) return;

  QHash<QString, rtEPPropertyPointList*>::const_iterator i = m_pointLists.constBegin();
  while (i != m_pointLists.constEnd()) {
    if(i.value()) delete i.value();
    ++i;
  }
}

void rtEPInfoObject::cleanupSphereList() {
  while(!m_sphereList.empty()) {
    vtkSphereSource* temp = m_sphereList.takeFirst();
    if(temp) temp->Delete();
  }
}

void rtEPInfoObject::cleanupSphereScalarList() {
  while(!m_sphereScalarList.empty()) {
    vtkDataArray* temp = m_sphereScalarList.takeFirst();
    if(temp) temp->Delete();
  }
}

void rtEPInfoObject::updatePointPolyData() {
  if (m_currentPropertyName=="") return;
  if (!m_pointLists.contains(m_currentPropertyName)) return;

  // Get the correct list
  rtEPPropertyPointList* ptList = m_pointLists.value(m_currentPropertyName);

  double propValueDiff = ptList->getMaxValue()-ptList->getMinValue();

  // Remove the old spheres from the list.
  cleanupSphereList();
  cleanupSphereScalarList();

  vtkSphereSource* temp;
  vtkPolyData* poly;
  vtkDataArray* scalars;
  double scalarValue;
  double currLocation[3];
  // Remove the previous inputs.
  m_pointPolyData->RemoveAllInputs();

  for (int ix1=0; ix1<ptList->getNumPoints(); ix1++) {
    rtNamedInfoPointData pt = ptList->getPointAt(ix1);
    temp = vtkSphereSource::New();
    scalars = vtkDataArray::CreateDataArray(VTK_DOUBLE);
    m_sphereList.append(temp);
    m_sphereScalarList.append(scalars);
    pt.getPoint(currLocation);
    temp->SetCenter(currLocation[0], currLocation[1], currLocation[2]);
    poly = temp->GetOutput();
    poly->Update();

    scalarValue = ((double)(pt.getValue(m_currentPropertyName) - ptList->getMinValue()))/propValueDiff;
    for (int ptNum=0; ptNum < poly->GetNumberOfPoints(); ptNum++) {
      scalars->InsertNextTuple(&scalarValue);
    }
    poly->GetPointData()->SetScalars(scalars);

    m_pointPolyData->SetInputByNumber(ix1, poly);
  }
}
