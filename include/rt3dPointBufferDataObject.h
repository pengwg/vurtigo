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

  //! Get a handle to the named info hash.
  inline QHash<int, rtNamedInfoPointData>* getNamedInfoHash() { return &m_namedInfoData; }

  //! Get the size of the hash
  inline int getNamedInfoHashSize() { return m_namedInfoData.size(); }

  //! Get the list of point IDs that are part of the selected list.
  /*!
    When the user selects a point, or a group of points from the GUI list the IDs of those points are added to this list.
    */
  inline QList<int>* getSelectedItemsList() { return &m_selectedItems; }

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

  inline void removeAllPoints() { m_pointList.clear(); m_namedInfoData.clear(); Modified(); }

  void applyTransformToPoints(vtkTransform * t);
  void applyTranslateToPoints(double x, double y, double z);


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

  //! Slot called when a new tag is added.
  void addNewTagButton();

  //! Slot called to update the table in the GUI
  void updateGuiPointList();

  //! Slot called when the table selection has been changed.
  void tableSelectionChanged();

  //! Slot called when the contents of a cell has changed.
  void tableCellChanged(int row, int col);

 protected:
  // Properties
  //! List of points in 3D space
  QList<rtBasic3DPointData> m_pointList;


  //! List of all the points with extra tag information.
  /*!
    Points are indexed by their ID. The ID should be unique for both lists.
    This list is what is displayed in the GUI.
  */
  QHash<int, rtNamedInfoPointData> m_namedInfoData;

  //! List of column headers.
  QList<QString> m_columnHeaderList;

  //! List of selected items.
  QList<int> m_selectedItems;

  //! Ui Interface
  Ui::rt3DPointBuffer m_optionsWidget;

  //! The current scale for this object
  double m_currentScale;

  // Functions
  //! Set the GUI widgets.
  /*!
  */
  void setupGUI();

  //! Clean the GUI widgets.
  /*!
  */
  void cleanupGUI();
  
  //! Get the next unique ID that has not yet been assigned.
  /*!
    Returns -1 if all of the IDs have been used.
    */
  int getNextId();

  //! Update the list of column headers based on the current points.
  void updateColumnHeaders();
};

#endif 
