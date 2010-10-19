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

#ifndef PLANE2PCONTROLWIDGET_H
#define PLANE2PCONTROLWIDGET_H

#include "ObjectControlWidget.h"

class Plane2DControlWidget : public ObjectControlWidget
{
public:
  Plane2DControlWidget();
  ~Plane2DControlWidget();

  virtual void sizeChanged();
  virtual void visibilityChanged();
  virtual void userTransformChanged();
  virtual void widgetOpacityChanged();

public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

protected:

  virtual void updateWidgetPosition();

  //! Get the actor at a given x,y location
  /*!
    If no actor is found the function returns NULL.
    The click position parameter is loaded with the coordinates in 3-space of the pick location.
    */
  vtkActor* getLocalPropAt(int x, int y, double clickPos[3]);


  // Central sphere.
  rtSingle3DPointPipeline m_centralSphere;

  // Crosshair
  rtCrosshair2D m_crosshair;

  // Box outline
  rtBox2DOutline m_boxOutline;

  // The three rings.
  vtkTransform *m_position[3];
  vtkParametricTorus *m_torus[3];
  vtkParametricFunctionSource *m_torusSrc[3];
  vtkPolyDataMapper *m_diskMapper[3];
  vtkActor *m_diskActor[3];

  //! The previous x location of the mouse
  int m_oldX;
  //! The previous y location of the mouse
  int m_oldY;

  //! The position of where the user just clicked.
  double m_clickPosition[3];

  //! The positive direction with respect to where on a wheel a user clicked.
  double m_positiveDirection[3];
};

#endif // PLANE2PCONTROLWIDGET_H
