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

  //! Get the size of the point list
  inline unsigned int getPointListSize() { return m_namedInfoData.keys().size(); }

  //! Get a handle to the named info hash.
  inline QHash<int, rtNamedInfoPointData>* getNamedInfoHash() { return &m_namedInfoData; }

  //! Get the size of the hash
  inline int getNamedInfoHashSize() { return m_namedInfoData.size(); }

  //! Get the list of point IDs that are part of the selected list.
  /*!
    When the user selects a point, or a group of points from the GUI list the IDs of those points are added to this list.
    */
  inline QList<int>* getSelectedItemsList() { return &m_selectedItems; }

  //! Get the filename this object is currently saved in
  inline QString getFilename() { return m_saveFileName;}

  //! Get a handle to the point at a particular location in 3D.
  /*!
    \param x The X coord of the point to find.
    \param y The Y coord of the point to find.
    \param z The Z coord of the point to find.
    \return The handle to the point or 0 if the point is not found.
    */
  rtNamedInfoPointData* getPointAt(double x, double y, double z);

  //! Get a handle to a point with a partiucular Id.
  /*!
    \return The handle to the point or 0 if the point is not found.
    */
  rtNamedInfoPointData* getPointWithId(int id);
  
  //! Remove the point with a given id.
  void removePointWithId(int id);

  //! Get a handle to a point at a particular index
  /*!
    \return The handle to the point or 0 if the point is not found.
    */
  rtNamedInfoPointData* getPointAtIndex(int index);

  void addPoint(rtBasic3DPointData sp);
  void addTimePoint(rt3DTimePointData pt);
  void addCartoPoint(rtCartoPointData pt);
  void addCustomPoint(rtBasic3DPointData pt,QList<QString> tags,QList<double> vals);
  void addCustomTimePoint(rt3DTimePointData pt,QList<QString> tags,QList<double> vals);
  void addCustomCartoPoint(rtCartoPointData pt,QList<QString> tags,QList<double> vals);

  inline void removeAllPoints() {
    m_namedInfoData.clear();
    m_columnHeaderList.clear();
    m_selectedItems.clear();
    m_currentScale = 1.0;
    emit pointListModifiedSignal();
  }

  void applyTransformToPoints(vtkTransform * t);
  void applyTranslateToPoints(double x, double y, double z);

  inline double getPointZoom() { return m_pointZoom; }
  inline void setPointZoom(double zoom) { m_optionsWidget.pointZoomSlider->setValue(zoom*10.0); }

  //! Get the center of the point list.
  void getPointListCenter(double center[3]);

  //! Get the sizes for the extents
  void getPointListExtents(double extents[6]);

  void update();

  //! Save this data object to file.
  virtual bool saveFile(QFile *file);

  //! Load this data object from file.
  virtual bool loadFile(QFile *file);

 signals:
  void pointListModifiedSignal();

 public slots:
  //! Slot called when the zoom slider is modified.
  void pointZoomChanged(int zoom);

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

  //! Slot called when the user chooses to clear the point data.
  void clearPointDataPressed();

  //! Slot called when the user wants to load an object into this object.
  void loadThisObject();

  //! Slot called when the user wants to save this object.
  void saveThisObject();
  //! Slot called when the user wants to save this object with a new filename
  void saveAsThisObject();

  //! Slot called when the object has been saved
  void savedObject();

  //! Slot called when the user wants to delete the selected points.
  void removeSelectedPoints();

  //! Slot called when the filter points button is pressed.
  void filterPoints();

  //! Slot called when the user wants to add a new point.
  void createNewPoint();

  //! Slot called when a new tag is added.
  void addNewTagButton();

  //! Slot called to update the table in the GUI
  void updateGuiPointList();

  //! Slot called when the table selection has been changed.
  void tableSelectionChanged();

  //! Slot called when the contents of a cell has changed.
  void tableCellChanged(int row, int col);

  //! Called when the point list is modified.
  /*!
    The function is not called directly because it may be that the points are modified in a thread other than the GUI thread.
    */
  void pointListModifiedSlot();

 protected:
  // Properties

  //! List of all the points with extra tag information.
  /*!
    Points are indexed by their ID. The ID should be unique.
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
  /*!
    The scaling does not change the size of each point. It does change the spread of a set of points.
    */
  double m_currentScale;

  //! The current zoom for this object.
  /*!
    The zoom changes the visible size of the point. The actual size is unchanged.
    */
  double m_pointZoom;

  //! The filename to save to if one has been chosen
  QString m_saveFileName;

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

  //! The buffer data tag has been reached.
  /*!
    \return The number of points to follow this buffer data.
    */
  int loadBufferData(QXmlStreamReader* reader);

  //! Each info point tag is read by this function.
  void loadInfoPoint(QXmlStreamReader* reader);
};

#endif 
