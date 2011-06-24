/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef PLANE2PCONTROLWIDGET_H
#define PLANE2PCONTROLWIDGET_H

#include "ObjectControlWidget.h"

#include "rtBox2DOutline.h"
#include "rtCrosshair2D.h"
#include "rtSingle3DPointPipeline.h"
#include "rtCompassWidgetPipeline.h"

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
  virtual void mousePressEvent(QMouseEvent* event, int window);
  virtual void mouseMoveEvent(QMouseEvent* event, int window);
  virtual void mouseReleaseEvent(QMouseEvent* event, int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event, int window);
  virtual void keyPressEvent(QKeyEvent* event, int window);
  virtual void keyReleaseEvent(QKeyEvent* event, int window);
  virtual void wheelEvent(QWheelEvent* event, int window);

protected:

  virtual void updateWidgetPosition();

  //! Get the actor at a given x,y location
  /*!
    If no actor is found the function returns NULL.
    The click position parameter is loaded with the coordinates in 3-space of the pick location.
    */
  vtkActor* getLocalPropAt(int x, int y, double clickPos[3], int window);


  // Central sphere.
  rtSingle3DPointPipeline m_centralSphere;
  vtkSphereSource *m_sphere;
  vtkPolyDataMapper *m_mapper;

  // Crosshair
  rtCrosshair2D m_crosshair;

  // Box outline
  rtBox2DOutline m_boxOutline;

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

#endif // PLANE2PCONTROLWIDGET_H
