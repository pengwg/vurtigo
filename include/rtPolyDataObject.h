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
#ifndef RT_POLY_DATA_OBJECT_H
#define RT_POLY_DATA_OBJECT_H

#include "rtDataObject.h"

#include <QList>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

//! Object that represents polygonal data
/*!
  Polygon Data object. Objects are composed of a set of points and a set of lines to connect those points.
  */
class rtPolyDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! A point that is part of a set in the poly data.
  struct PolyPoint {
    //! The XYZ coordinate list
    double ptList[3];
    //! The RGB color for this point.
    int color[3];
  };

  //! A link between two point the poly data.
  /*!
    To create polygonal data the points must be joined into polygons that will be rendered as surfaces.
To join three points create an object such as this one and give the number of each point. Numbering starts at ZERO.
Points should be joined into triangles to be rendered.
    */
  struct PolyPointLink {
    //! The three poly points that make up the polygon.
    int threeVertex[3];
  };

  rtPolyDataObject();
  ~rtPolyDataObject();

  void update();

  vtkPolyData* getPolyData() { return m_polyData; }
  vtkProperty* getProperty() { return m_polyProperty; }
  vtkColorTransferFunction* getColorTable() { return m_colorLookup; }

  bool setNewGeometry(QList<PolyPoint> *pts, QList<PolyPointLink> *links);
  bool copyPolyData(vtkPolyData*);
  bool copyLookupTable(vtkColorTransferFunction*);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkPolyData> m_polyData;
  vtkSmartPointer<vtkProperty> m_polyProperty;
  vtkSmartPointer<vtkColorTransferFunction> m_colorLookup;

};

#endif 
