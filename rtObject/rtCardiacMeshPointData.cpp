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

#include "rtCardiacMeshPointData.h"

rtCardiacMeshPointData::rtCardiacMeshPointData()
 : m_location(0), m_slice(0)
{
}

rtCardiacMeshPointData::rtCardiacMeshPointData(const rtCardiacMeshPointData& sp)
 : rt3DTimePointData(sp)
{
  m_location = sp.m_location;
  m_slice = sp.m_slice;
}

bool rtCardiacMeshPointData::operator==(const rtCardiacMeshPointData &other) const {
  return rt3DTimePointData::operator==(other) && m_location == other.m_location && m_slice == other.m_slice;
}

rtCardiacMeshPointData& rtCardiacMeshPointData::operator=(const rtCardiacMeshPointData& sp) {
  if (this!=&sp) {
    rt3DTimePointData::operator=(sp);
    m_location = sp.m_location;
    m_slice = sp.m_slice;
  }
  return *this;
}
