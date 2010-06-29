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
#include "rtBasic3DPointData.h"
#include "ui_rt3DPointBuffer.h"

#include <QList>
#include <QColor>

#include <vtkProperty.h>
#include <vtkTransform.h>

//! A buffer of 3D points
class rt3DPointBufferDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt3DPointBufferDataObject();
  ~rt3DPointBufferDataObject();

  QList<rtBasic3DPointData>* getPointList() { return &m_pointList; }

  //! Get a handle to the point at a particular location in 3D.
  /*!
    \param x The X coord of the point to find.
    \param y The Y coord of the point to find.
    \param z The Z coord of the point to find.
    \return The handle to the point or 0 if the point is not found.
    */
  rtBasic3DPointData* getPointAt(double x, double y, double z);

  //! Get a handle to a point with a partiucular Id.
  /*!
    \return The handle to the point or 0 if the point is not found.
    */
  rtBasic3DPointData* getPointWithId(int id);

  vtkTransform* const getTransform() { return m_pTransform; }

  void addPoint(rtBasic3DPointData sp);
  void removePoint(rtBasic3DPointData sp);

  inline void removeAllPoints() { m_pointList.clear(); Modified(); }

  void update();

 public slots:
  void identityButton();
  void transPlusX();
  void transMinusX();
  void transPlusY();
  void transMinusY();
  void transPlusZ();
  void transMinusZ();
  void rotPlusX();
  void rotMinusX();
  void rotPlusY();
  void rotMinusY();
  void rotPlusZ();
  void rotMinusZ();
  void scaleChanged(double val);

 protected:
  // Properties
  //! List of points in 3D space
  QList<rtBasic3DPointData> m_pointList;

  //! Global transform for all of the points.
  vtkTransform* m_pTransform;

  //! Ui Interface
  Ui::rt3DPointBuffer m_optionsWidget;

  //! The current scale for this object
  double m_currentScale;

  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
