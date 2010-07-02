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

#include "rt3DTimePointData.h"

rt3DTimePointData::rt3DTimePointData()
 : m_triggerDelay(0), m_phase(0)
{
}

rt3DTimePointData::rt3DTimePointData(const rt3DTimePointData& sp)
 : rtBasic3DPointData(sp)
{
  m_triggerDelay = sp.m_triggerDelay;
  m_phase = sp.m_phase;
}


bool rt3DTimePointData::operator==(const rt3DTimePointData &other) const {
  return rtBasic3DPointData::operator==(other) && m_triggerDelay == other.m_triggerDelay && m_phase == other.m_phase;
}

rt3DTimePointData& rt3DTimePointData::operator=(const rt3DTimePointData& sp) {
  if (this!=&sp) {
    rtBasic3DPointData::operator=(sp);
    m_triggerDelay = sp.m_triggerDelay;
    m_phase = sp.m_phase;
  }
  return *this;
}
