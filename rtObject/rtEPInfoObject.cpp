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
{
  m_defaultColorFunc = vtkColorTransferFunction::New();
  m_defaultColorFunc->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  m_defaultColorFunc->AddRGBPoint(0.5, 0.5, 0.5, 0.5);
  m_defaultColorFunc->AddRGBPoint(1.0, 1.0, 0.0, 0.0);

  m_currentPropertyName = "";
}

rtEPInfoObject::~rtEPInfoObject() {
  m_defaultColorFunc->Delete();

  cleanupHash();
}


void rtEPInfoObject::addInfoPoint(rtEPPropertyPointList::InfoPoint p, QString propName) {
  if (m_pointLists.isEmpty()) {
    rtEPPropertyPointList* temp = new rtEPPropertyPointList(propName);
    m_pointLists.insert(propName, temp);
    temp->addPointToList(p);
    m_currentPropertyName = propName;
  } else if(m_pointLists.contains(propName)) {
    m_pointLists.value(propName)->addPointToList(p);
  } else {
    rtEPPropertyPointList* temp = new rtEPPropertyPointList(propName);
    m_pointLists.insert(propName, temp);
    temp->addPointToList(p);
  }
}

bool rtEPInfoObject::getInfoPoint(double x, double y, double z, rtEPPropertyPointList::InfoPoint &p, QString propName) {
  if(m_pointLists.contains(propName)) {
    return m_pointLists.value(propName)->getInfoPoint(x, y, z, p);
  } else {
    return false;
  }
}

bool rtEPInfoObject::removeInfoPoint(double x, double y, double z, rtEPPropertyPointList::InfoPoint &p, QString propName) {
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
  unsigned int proximalPoints;
  double tempPT[3];
  double currDist;
  double propValueDiff = ptList->getMaxValue()-ptList->getMinValue();

  numPts = data->GetNumberOfPoints();
  vtkFloatArray * scalars = vtkFloatArray::New();

  for (vtkIdType currID=0; currID<numPts; currID++) {
    proximalPoints = 1;
    val = 0.5;
    data->GetPoint(currID, tempPT);

    for (int ix1=0; ix1<ptList->getNumPoints(); ix1++) {
      rtEPPropertyPointList::InfoPoint pt = ptList->getPointAt(ix1);
      currDist = vtkMath::Distance2BetweenPoints(tempPT, pt.location);
      if (currDist < 20.0) {
        val += ((double)(pt.property - ptList->getMinValue()))/propValueDiff;
        proximalPoints++;
      }
    }
    val = val/((float)proximalPoints);
    scalars->InsertTuple(currID, &val);

  }
  data->GetPointData()->SetScalars(scalars);
  scalars->Delete();

  return true;
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
