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
#include "rtCameraControl.h"
#include "customQVTKWidget.h"
#include <iostream>
rtCameraControl::rtCameraControl(vtkCamera* cam, customQVTKWidget* eventWid) {
  m_camera = cam;
  m_eventWidget = eventWid;

  connectEvents();

  m_leftMouseDown = false;
  m_rightMouseDown = false;
  m_midMouseDown = false;
}

void rtCameraControl::connectEvents() {
  if (!m_eventWidget) return;
  connect(m_eventWidget, SIGNAL(cameraMousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
  connect(m_eventWidget, SIGNAL(cameraMouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
  connect(m_eventWidget, SIGNAL(cameraMouseRelease(QMouseEvent*)), this, SLOT(mouseRelease(QMouseEvent*)));
  connect(m_eventWidget, SIGNAL(cameraMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClick(QMouseEvent*)));
  connect(m_eventWidget, SIGNAL(cameraKeyPress(QKeyEvent*)), this, SLOT(keyPress(QKeyEvent*)));
  connect(m_eventWidget, SIGNAL(cameraKeyRelease(QKeyEvent*)), this, SLOT(keyRelease(QKeyEvent*)));
  connect(m_eventWidget, SIGNAL(cameraWheel(QWheelEvent*)), this, SLOT(wheel(QWheelEvent*)));
}


void rtCameraControl::mousePress(QMouseEvent* ev) {
  if (ev->button() == Qt::LeftButton) {
    m_leftMouseDown = true;
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = true;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = true;
  }


}

void rtCameraControl::mouseMove(QMouseEvent* ev) {
  if (m_leftMouseDown) {
    // Rotate the camera.
  }
}

void rtCameraControl::mouseRelease(QMouseEvent* ev) {
  if (ev->button() == Qt::LeftButton) {
    m_leftMouseDown = false;
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = false;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = false;
  }
}

void rtCameraControl::mouseDoubleClick(QMouseEvent* ev) {
}

void rtCameraControl::keyPress(QKeyEvent* ev) {
}

void rtCameraControl::keyRelease(QKeyEvent* ev) {
}

void rtCameraControl::wheel(QWheelEvent* ev) {
}
