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
#ifndef RT_3D_POINT_BUFFER_DATA_OBJECT_H
#define RT_3D_POINT_BUFFER_DATA_OBJECT_H

#include "rtDataObject.h"

#include <QList>
#include <QColor>

#include "vtkProperty.h"

//! A buffer of 3D points
class rt3DPointBufferDataObject : public rtDataObject
{
Q_OBJECT

public:

  class SimplePoint {
  public:
    double px, py, pz;
    double pSize;
    vtkProperty *pProp;

    SimplePoint() {
      pProp = vtkProperty::New();
    }

    ~SimplePoint() {
      pProp->Delete();
    }

    bool operator==(const SimplePoint &other) const {
      if (px==other.px && 
	  py==other.py && 
	  pz==other.pz && 
	  pSize == other.pSize && 
	  pProp == other.pProp) 
	return true;
      else 
	return false;
    }

    SimplePoint(const SimplePoint& sp) {
      px = sp.px;
      py = sp.py;
      pz = sp.pz;
      pSize = sp.pSize;

      pProp = vtkProperty::New();
      pProp->DeepCopy(sp.pProp);
    }

    SimplePoint& operator=(const SimplePoint& sp) {
      if (this == &sp)      // Same object?
	return *this;

      px = sp.px;
      py = sp.py;
      pz = sp.pz;
      pSize = sp.pSize;

      pProp->Delete();
      pProp = vtkProperty::New();
      pProp->DeepCopy(sp.pProp);
    }

  };

  rt3DPointBufferDataObject();
  ~rt3DPointBufferDataObject();

  QList<SimplePoint>* getPointList() { return &m_pointList; }
  void addPoint(SimplePoint sp);
  void removePoint(SimplePoint sp);

  void update();

 protected:
  // Properties
  QList<SimplePoint> m_pointList;

  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
