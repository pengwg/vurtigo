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

#include <vtkProperty.h>

#include "rtBasicPointData.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

rtBasicPointData::rtBasicPointData()
 : m_pId(-1), m_pSize(1.0), m_pProp(0)
{
  m_coords.clear();
  m_pProp = vtkProperty::New();
  m_timeStamp = rtApplication::instance().getTimeManager()->getSystemTime();
  m_redC = 0.5;
  m_greenC = 0.5;
  m_blueC = 0.5;
}

rtBasicPointData::rtBasicPointData(const rtBasicPointData& sp) {
  m_pId = sp.m_pId;
  m_coords = sp.m_coords;
  m_pSize = sp.m_pSize;
  m_timeStamp = sp.m_timeStamp;
  m_pProp = vtkProperty::New();
  m_pProp->DeepCopy(sp.m_pProp);
  m_redC = sp.m_redC;
  m_greenC = sp.m_greenC;
  m_blueC = sp.m_blueC;
}

rtBasicPointData::~rtBasicPointData() {
  if(m_pProp) m_pProp->Delete();
}

bool rtBasicPointData::operator==(const rtBasicPointData &other) const {
  if (m_pId==other.m_pId &&  m_coords==other.m_coords && m_pSize == other.m_pSize && m_pProp == other.m_pProp && m_timeStamp==other.m_timeStamp
      && m_redC == other.m_redC &&  m_greenC == other.m_greenC &&  m_blueC == other.m_blueC )
    return true;
  else
    return false;
}

rtBasicPointData& rtBasicPointData::operator=(const rtBasicPointData& sp) {
  if (this != &sp) {
    m_pId = sp.m_pId;
    m_coords = sp.m_coords;
    m_pSize = sp.m_pSize;
    m_timeStamp = sp.m_timeStamp;
    if(m_pProp) m_pProp->Delete();
    m_pProp = vtkProperty::New();
    m_pProp->DeepCopy(sp.m_pProp);
    m_redC = sp.m_redC;
    m_greenC = sp.m_greenC;
    m_blueC = sp.m_blueC;
  }

  return *this;
}
