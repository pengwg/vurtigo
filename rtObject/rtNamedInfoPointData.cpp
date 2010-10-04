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

#include "rtNamedInfoPointData.h"
#include "rtCartoPointData.h"

rtNamedInfoPointData::rtNamedInfoPointData()
{
  m_infoMap.clear();
}

rtNamedInfoPointData::rtNamedInfoPointData(const rtNamedInfoPointData& sp)
 : rtBasic3DPointData(sp)
{
  m_infoMap = sp.m_infoMap;
}

bool rtNamedInfoPointData::operator==(const rtNamedInfoPointData &other) const {
  return rtBasic3DPointData::operator==(other) && m_infoMap == other.m_infoMap;
}

rtNamedInfoPointData& rtNamedInfoPointData::operator=(const rtNamedInfoPointData& sp) {
  if (this!=&sp) {
    rtBasic3DPointData::operator=(sp);
    m_infoMap = sp.m_infoMap;
  }
  return *this;
}

void rtNamedInfoPointData::fromBasic3DPoint(rtBasic3DPointData* pt) {
  rtBasic3DPointData::operator=( (*pt) );
}

void rtNamedInfoPointData::fromCartoPoint(rtCartoPointData* pt) {
  rtBasic3DPointData::operator=( (*pt) );
  setNamedValue("Alpha", pt->getAlpha());
  setNamedValue("Beta", pt->getBeta());
  setNamedValue("Gamma", pt->getGamma());
  setNamedValue("UniPolar", pt->getUniPolar());
  setNamedValue("BiPolar", pt->getBiPolar());
  setNamedValue("LAT", pt->getLAT());
}
