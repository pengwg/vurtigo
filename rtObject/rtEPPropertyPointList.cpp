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

#include "rtEPPropertyPointList.h"

rtEPPropertyPointList::rtEPPropertyPointList(QString name)
: m_maxPropValue(-1), m_minPropValue(-1)
{
  m_propName = name;
  m_infoList.clear();
}

rtEPPropertyPointList::~rtEPPropertyPointList() {
}


void rtEPPropertyPointList::addPointToList(rtNamedInfoPointData p) {
  if (m_infoList.empty()) {
    m_maxPropValue = p.getValue();
    m_minPropValue = p.getValue();
    m_infoList.append(p);
  } else {
    m_infoList.append(p);
    if (p.getValue() > m_maxPropValue) m_maxPropValue = p.getValue();
    if (p.getValue() < m_minPropValue) m_minPropValue = p.getValue();
  }
}

bool rtEPPropertyPointList::getInfoPoint(double x,double y,double z, rtNamedInfoPointData&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() && !found; ix1++) {
    if (m_infoList[ix1].getX() == x && m_infoList[ix1].getY() == y && m_infoList[ix1].getZ() == z) {
      p = m_infoList.at(ix1);
      found = true;
    }
  }
  return found;
}


bool rtEPPropertyPointList::removeInfoPoint(double x,double y,double z,rtNamedInfoPointData&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() &&!found; ix1++) {
    if (m_infoList[ix1].getX() == x && m_infoList[ix1].getY() == y && m_infoList[ix1].getZ() == z) {
      p = m_infoList.takeAt(ix1);
      found = true;
    }
  }
  return found;
}


void rtEPPropertyPointList::clearPointList() {
  m_infoList.clear();
  m_maxPropValue = -1;
  m_minPropValue = -1;
}
