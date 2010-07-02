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

#ifndef RTCARDIACMESHPOINTDATA_H
#define RTCARDIACMESHPOINTDATA_H

#include "rt3DTimePointData.h"

class rtCardiacMeshPointData : public rt3DTimePointData
{
public:
  rtCardiacMeshPointData();

  //! Copy constructor
  rtCardiacMeshPointData(const rtCardiacMeshPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtCardiacMeshPointData &other) const;
  //! Assignment operator
  rtCardiacMeshPointData& operator=(const rtCardiacMeshPointData& sp);


  inline void setLocation(unsigned int loc) { m_location=loc; }
  inline void setSlice(unsigned int slice) { m_slice = slice; }

  inline unsigned int getLocation() { return m_location; }
  inline unsigned int getSlice() { return m_slice; }

protected:
  //! Location order of the point on the given slice.
  unsigned int m_location;

  //! Slice number that this point lies on.
  unsigned int m_slice;
};

#endif // RTCARDIACMESHPOINTDATA_H
