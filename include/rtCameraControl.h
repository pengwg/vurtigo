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
#ifndef RT_CAMERA_CONTROL_H
#define RT_CAMERA_CONTROL_H

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPoint>

#include "vtkCamera.h"

class customQVTKWidget;

class rtCameraControl : public QObject {
  Q_OBJECT
public:
  rtCameraControl(vtkCamera* cam, customQVTKWidget* eventWidget);

  bool cameraMoving();

public slots:
  void mousePress(QMouseEvent*);
  void mouseMove(QMouseEvent*);
  void mouseRelease(QMouseEvent*);
  void mouseDoubleClick(QMouseEvent*);
  void keyPress(QKeyEvent*);
  void keyRelease(QKeyEvent*);
  void wheel(QWheelEvent*);

protected:
  vtkCamera* m_camera;
  customQVTKWidget* m_eventWidget;
  QPoint m_lastPoint;

  bool m_leftMouseDown;
  bool m_rightMouseDown;
  bool m_midMouseDown;

  void connectEvents();
};

#endif // RT_CAMERA_CONTROL_H
