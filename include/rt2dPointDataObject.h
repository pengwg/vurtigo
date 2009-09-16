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
#ifndef RT_2D_POINT_DATA_OBJECT_H
#define RT_2D_POINT_DATA_OBJECT_H

#include "rtDataObject.h"

//! Object that represents a set of 2D points
/*!
  A data representation of a point in 2D space.
  */
class rt2DPointDataObject : public rtDataObject
{
Q_OBJECT

public:
  //! A representation of a 2D point.
  class SimplePoint2D{
    public:
      int x,y;

    bool operator==(const SimplePoint2D &other) const {
      if (x==other.x && y==other.y)
        return true;
      else
        return false;
    }

    SimplePoint2D(const SimplePoint2D& sp) {
      x = sp.x;
      y = sp.y;
    }

    SimplePoint2D& operator=(const SimplePoint2D& sp) {
      if (this == &sp)      // Same object?
        return *this;
      x = sp.x;
      y = sp.y;
    }
  };

  rt2DPointDataObject();
  ~rt2DPointDataObject();

  QList<SimplePoint2D>* getPointList() { return &m_pointList; }
  void addPoint(SimplePoint2D sp);
  void removePoint(SimplePoint2D sp);

  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  QList<SimplePoint2D> m_pointList;
};

#endif 
