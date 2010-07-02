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

rtNamedInfoPointData::rtNamedInfoPointData()
 : m_namedInfo(0.0), m_name("")
{
}

rtNamedInfoPointData::rtNamedInfoPointData(const rtNamedInfoPointData& sp)
 : rtBasic3DPointData(sp)
{
  m_namedInfo = sp.m_namedInfo;
  m_name = sp.m_name;
}

bool rtNamedInfoPointData::operator==(const rtNamedInfoPointData &other) const {
  return rtBasic3DPointData::operator==(other) && m_namedInfo == other.m_namedInfo && m_name == other.m_name;
}

rtNamedInfoPointData& rtNamedInfoPointData::operator=(const rtNamedInfoPointData& sp) {
  if (this!=&sp) {
    rtBasic3DPointData::operator=(sp);
    m_namedInfo = sp.m_namedInfo;
    m_name = sp.m_name;
  }
  return *this;
}
