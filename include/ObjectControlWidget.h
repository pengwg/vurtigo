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

#ifndef OBJECTCONTROLWIDGET_H
#define OBJECTCONTROLWIDGET_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <vtkTransform.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkParametricTorus.h>
#include <vtkParametricFunctionSource.h>

//! Base class for widgets that control the movement of objects in 3D.
class ObjectControlWidget : public QObject
{
  Q_OBJECT
public:
  ObjectControlWidget();
  ~ObjectControlWidget();

  void setTransform(vtkTransform* input);
  void getTransform(vtkTransform* output);

  void setSize(double xsize, double ysize, double zsize=0);
  inline double getXSize() { return m_xsize; }
  inline double getYSize() { return m_ysize; }
  inline double getZSize() { return m_zsize; }

  //! Function is called to force recalculation with new sizes.
  virtual void sizeChanged();

  void show();
  void hide();
  bool isShowing();

  //! Function called when the visibility flag has changed.
  virtual void visibilityChanged();

  void setUserTransform(vtkTransform* t);
  virtual void userTransformChanged();

  void setWidgetOpacity(double op);
  double getWidgetOpacity();
  virtual void widgetOpacityChanged();

  inline vtkActor* getObjectOfInterest() { return m_objOfInterest; }
  inline void setObjectOfInterest(vtkActor* obj) { m_objOfInterest = obj; }

public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

protected:
  //! Determine if this widget is being shown
  bool m_showing;

  vtkTransform* m_transform;
  vtkTransform* m_userTransform;
  double m_xsize, m_ysize, m_zsize;

  //! The untranslated point locations as a grid.
  double m_pointLocations[11][3];

  //! The translated points into the new space.
  double m_convertedLocations[11][3];

  //! The central corners of the grid.
  int m_corners[4];

  vtkActor *m_currProp;

  //! The opacity of the widget.
  double m_widgetOpacity;

  //! The object that is going to be moved
  /*!
    This is used for the opacity picking.
    */
  vtkActor* m_objOfInterest;

  //! Function called to do the update of the widget.
  virtual void updateWidgetPosition();


  //! Update the converted locations array.
  /*!
    The update uses the point locations and the transform.
    */
  void updateConvertedLocations();


  //! Check if the object of interest is under the position x, y
  bool pickedObjectOfInterest(int x, int y);
};

#endif // OBJECTCONTROLWIDGET_H
