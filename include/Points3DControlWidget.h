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

#ifndef POINTS3DCONTROLWIDGET_H
#define POINTS3DCONTROLWIDGET_H

#include "ObjectControlWidget.h"

#include "rtBox2DOutline.h"
#include "rtCrosshair2D.h"
#include "rtSingle3DPointPipeline.h"
#include "rtCompassWidgetPipeline.h"

class Points3DControlWidget : public ObjectControlWidget
{
public:
  Points3DControlWidget();
  ~Points3DControlWidget();

  virtual void sizeChanged();
  virtual void visibilityChanged();
  virtual void userTransformChanged();
  virtual void widgetOpacityChanged();

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event,int window);
  virtual void mouseReleaseEvent(QMouseEvent* event,int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event,int window);
  virtual void keyPressEvent(QKeyEvent* event,int window);
  virtual void keyReleaseEvent(QKeyEvent* event,int window);
  virtual void wheelEvent(QWheelEvent* event,int window);

protected:

  virtual void updateWidgetPosition();

  //! Get the actor at a given x,y location
  /*!
    If no actor is found the function returns NULL.
    The click position parameter is loaded with the coordinates in 3-space of the pick location.
    */
  vtkActor* getLocalPropAt(int x, int y, double clickPos[3],int window);


  // Central sphere.
  rtSingle3DPointPipeline m_centralSphere;

  // Crosshair
  rtCrosshair2D m_crosshair;

  // The three rings.
  rtCompassWidgetPipeline m_compassWidget;

  //! The previous x location of the mouse
  int m_oldX;
  //! The previous y location of the mouse
  int m_oldY;

  //! The position of where the user just clicked.
  double m_clickPosition[3];

  //! The positive direction with respect to where on a wheel a user clicked.
  double m_positiveDirection[3];
};

#endif // POINTS3DCONTROLWIDGET_H
