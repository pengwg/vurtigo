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
    : m_maxPropValue(-1), m_minPropValue(-1)
{
  m_defaultColorFunc = vtkColorTransferFunction::New();
  m_defaultColorFunc->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  m_defaultColorFunc->AddRGBPoint(0.5, 0.5, 0.5, 0.5);
  m_defaultColorFunc->AddRGBPoint(1.0, 1.0, 0.0, 0.0);

  m_infoList.clear();
}

rtEPInfoObject::~rtEPInfoObject() {

  m_defaultColorFunc->Delete();
  m_infoList.clear();
}


void rtEPInfoObject::addInfoPoint(InfoPoint p) {
  if (m_infoList.empty()) {
    m_maxPropValue = p.property;
    m_minPropValue = p.property;
    m_infoList.append(p);
  } else {
    m_infoList.append(p);
    if (p.property > m_maxPropValue) m_maxPropValue = p.property;
    if (p.property < m_minPropValue) m_minPropValue = p.property;
  }
}

bool rtEPInfoObject::getInfoPoint(double x, double y, double z, InfoPoint &p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size(); ix1++) {
    if (m_infoList.at(ix1).location[0] == x && m_infoList.at(ix1).location[1] == y && m_infoList.at(ix1).location[2] == z) {
      p = m_infoList.at(ix1);
      found = true;
    }
  }
  return found;
}

void rtEPInfoObject::clearPointList() {
  m_infoList.clear();
  m_maxPropValue = -1;
  m_minPropValue = -1;
}

bool rtEPInfoObject::updateScalars(vtkPolyData* data) {
  if (!data) return false;

  // There must be at least TWO points of info for this to work.
  if (m_infoList.size() < 2) return true;

  // If the max and the min are equal then there is no point in doing this.
  if (m_maxPropValue == m_minPropValue) return true;

  float val;
  vtkIdType numPts;
  unsigned int proximalPoints;
  double tempPT[3];
  double currDist;
  double propValueDiff = m_maxPropValue-m_minPropValue;

  numPts = data->GetNumberOfPoints();
  vtkFloatArray * scalars = vtkFloatArray::New();

  for (vtkIdType currID=0; currID<numPts; currID++) {
    proximalPoints = 1;
    val = 0.5;
    data->GetPoint(currID, tempPT);

    for (int ix1=0; ix1<m_infoList.size(); ix1++) {
      currDist = vtkMath::Distance2BetweenPoints(tempPT, m_infoList[ix1].location);
      if (currDist < 20.0) {
        val += ((double)(m_infoList[ix1].property - m_minPropValue))/propValueDiff;
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
