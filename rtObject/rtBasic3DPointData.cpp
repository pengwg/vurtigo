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

#include "rtBasic3DPointData.h"

rtBasic3DPointData::rtBasic3DPointData()
{
  this->setDimention(3);
}

rtBasic3DPointData::rtBasic3DPointData(const rtBasic3DPointData& sp)
 : rtBasicPointData(sp)
{
  m_coords[0] = sp.m_coords[0];
  m_coords[1] = sp.m_coords[1];
  m_coords[2] = sp.m_coords[2];
}

bool rtBasic3DPointData::operator==(const rtBasic3DPointData &other) const {
  return rtBasicPointData::operator==(other) && m_coords[0]==other.m_coords[0] && m_coords[1]==other.m_coords[1] && m_coords[2]==other.m_coords[2];
}

rtBasic3DPointData& rtBasic3DPointData::operator=(const rtBasic3DPointData& sp) {
  if (this!=&sp) {
    rtBasicPointData::operator=(sp);
    m_coords[0]=sp.m_coords[0];
    m_coords[1]=sp.m_coords[1];
    m_coords[2]=sp.m_coords[2];
  }
  return *this;
}
