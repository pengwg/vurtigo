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

#ifndef RTBASIC3DPOINTDATA_H
#define RTBASIC3DPOINTDATA_H

#include <vtkTransform.h>

#include "rtBasicPointData.h"

class rtBasic3DPointData : public rtBasicPointData
{
public:
  rtBasic3DPointData();
  ~rtBasic3DPointData();

  //! Copy constructor
  rtBasic3DPointData(const rtBasic3DPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtBasic3DPointData &other) const;
  //! Assignment operator
  rtBasic3DPointData& operator=(const rtBasic3DPointData& sp);

  inline void setX(double x) { m_coords[0] = x; }
  inline void setY(double y) { m_coords[1] = y; }
  inline void setZ(double z) { m_coords[2] = z; }

  double getX();
  double getY();
  double getZ();
  void getPoint(double p[3]);
  void getTransformedPoint(double p[3]);
  
  void setPoint(double x, double y, double z);
  void setPoint(double p[3]);

  inline vtkTransform* getTransform() { return m_pTransform; }
  
  static double findDistance(rtBasic3DPointData p1, rtBasic3DPointData p2);
  

protected:
  vtkTransform *m_pTransform;
};

#endif // RTBASIC3DPOINTDATA_H
