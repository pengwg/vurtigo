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
#include "rtCartoPointData.h"
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

  //! Get the handle to the point list.
  inline QList<rtBasic3DPointData>* getPointList() { return &m_pointList; }

  //! Get the size of the point list
  inline unsigned int getPointListSize() { return m_pointList.size(); }

  //! Get a handle to the carto point list.
  inline QList<rtCartoPointData>* getCartoPointList() { return &m_cartoPointList; }

  //! Get the number of carto points available.
  inline unsigned int getCartoPointListSize() { return m_cartoPointList.size(); }

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
  
  //! Get a handle to a point at a particular index
  /*!
    \return The handle to the point or 0 if the point is not found.
    */
  rtBasic3DPointData* getPointAtIndex(int index);

  void addPoint(rtBasic3DPointData sp);
  void removePoint(rtBasic3DPointData sp);

  void addCartoPoint(rtCartoPointData pt);
  void removeCartoPoint(rtCartoPointData pt);

  inline void removeAllPoints() { m_pointList.clear(); Modified(); }
  inline void removeAllCartoPoints() { m_cartoPointList.clear(); Modified(); }

  void applyTransformToPoints(vtkTransform * t);
  void applyTransformToCartoPoints(vtkTransform * t);

  void applyTranslateToPoints(double x, double y, double z);
  void applyTranslateToCartoPoints(double x, double y, double z);

  void update();

 public slots:
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

  //! List of carto points in space
  QList<rtCartoPointData> m_cartoPointList;

  //! Ui Interface
  Ui::rt3DPointBuffer m_optionsWidget;

  //! The current scale for this object
  double m_currentScale;

  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
