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

#ifndef RTBASIC2DPOINTDATA_H
#define RTBASIC2DPOINTDATA_H

#include "rtBasicPointData.h"

class rtBasic2DPointData : public rtBasicPointData
{
public:
  rtBasic2DPointData();

  //! Copy constructor
  rtBasic2DPointData(const rtBasic2DPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtBasic2DPointData &other) const;

  //! Assignment operator
  rtBasic2DPointData& operator=(const rtBasic2DPointData& sp);

  inline void setX(double x) { m_coords[0] = x; }
  inline void setY(double y) { m_coords[1] = y; }

  inline double getX() { return m_coords[0]; }
  inline double getY() { return m_coords[1]; }
};

#endif // RTBASIC2DPOINTDATA_H
