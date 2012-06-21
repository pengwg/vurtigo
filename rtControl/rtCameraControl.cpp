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
#include <iostream>

#include "rtCameraControl.h"
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtMainWindow.h"
#include "customQVTKWidget.h"

rtCameraControl::rtCameraControl(vtkCamera* cam, vtkRenderer* renderer, customQVTKWidget* eventWid)
  : m_camera(cam), m_renderer(renderer), m_eventWidget(eventWid), m_leftMouseDown(false), m_rightMouseDown(false), m_midMouseDown(false), m_scrollWheelMotion(true), m_cameraInMotion(false)
{
  connectEvents();
  m_cameraPosList.clear();

  m_scrollTimer.setSingleShot(true);
  m_scrollTimer.setInterval(400); // Wait 400 msec before declaring the scroll event over.

  CameraPosition p;

  // Default Positions
  p.pos[0] = 0.0f; p.pos[1] = 0.0f; p.pos[2] = -1000.0f;
  p.focal[0] = 0.0f; p.focal[1] = 0.0f; p.focal[2] = 0.0f;
  p.up[0] = 0.0f; p.up[1] = -1.0f; p.up[2] = 0.0f;
  m_cameraPosList.append(p);

  // Robot Arm Position
  p.pos[0] = 0.0f; p.pos[1] = 0.0f; p.pos[2] = -1000.0f;
  p.focal[0] = 0.0f; p.focal[1] = 0.0f; p.focal[2] = 0.0f;
  p.up[0] = 0.0f; p.up[1] = -1.0f; p.up[2] = 0.0f;
  m_cameraPosList.append(p);
}

bool rtCameraControl::cameraMoving() {
  return (m_leftMouseDown || m_rightMouseDown || m_midMouseDown || m_scrollWheelMotion || m_cameraInMotion);
}

void rtCameraControl::getForwardDirection(double val[3]) {
  m_camera->OrthogonalizeViewUp();
  m_camera->GetDirectionOfProjection(val);
}

void rtCameraControl::getUpDirection(double val[3]) {
  m_camera->OrthogonalizeViewUp();
  m_camera->GetViewUp(val);
}

void rtCameraControl::getRightDirection(double val[3]) {
  m_camera->OrthogonalizeViewUp();

  double forward[3];
  double up[3];
  m_camera->GetDirectionOfProjection(forward);
  m_camera->GetViewUp(up);

  val[0] = forward[1]*up[2]-forward[2]*up[1];
  val[1] = forward[2]*up[0]-forward[0]*up[2];
  val[2] = forward[0]*up[1]-forward[1]*up[0];
}


void rtCameraControl::setToDefaultPosition() {

  m_camera->SetPosition(m_cameraPosList[0].pos);
  m_camera->SetFocalPoint(m_cameraPosList[0].focal);
  m_camera->SetViewUp(m_cameraPosList[0].up);

}

void rtCameraControl::setToRobotArmPosition() {
  m_camera->SetPosition(m_cameraPosList[1].pos);
  m_camera->SetFocalPoint(m_cameraPosList[1].focal);
  m_camera->SetViewUp(m_cameraPosList[1].up);
}

void rtCameraControl::setViewAngle(double angle) {
  if (!m_camera) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("VTK Camera pointer is NULL."));
    return;
  }

  double factor;
  // It is possible that the angle is too small or too large so it can be fixed here.
  if (angle < 0.0) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("View angle is negative. Adding sets of 180."));
    factor = (-1.0)*floor(angle / 180.0);
    angle = angle + 180.0*factor;
  } else if (angle > 180.0) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("View angle is greater than 180. Subtracting sets of 180."));
    factor = floor(angle / 180.0);
    angle = angle - 180.0*factor;
  }

  if ( angle != m_camera->GetViewAngle() ) {
    m_camera->SetViewAngle(angle);
    m_renderer->ResetCameraClippingRange();
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void rtCameraControl::setViewAngle(double elevation, double azimuth)
{
    m_camera->OrthogonalizeViewUp();
    m_camera->Elevation(elevation);
    m_camera->Azimuth(azimuth);
    m_renderer->ResetCameraClippingRange();
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

double rtCameraControl::getViewAngle() {
  if (!m_camera) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("VTK Camera pointer is NULL."));
    return 0.0;
  }
  return m_camera->GetViewAngle();
}

//////////////
// PUBLIC SLOTS
///////////////
void rtCameraControl::mousePress(QMouseEvent* ev,int window) {
  double vp[4];
  m_renderer->GetViewport(vp);

  m_lastPoint = ev->pos();

  // Convert the Qt x, y to VTK viewport.
  QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->size();
  double X = ((double)ev->x()/(double)winSize.width());
  double Y = ((double)(winSize.height()-ev->y())/(double)winSize.height());

  // Ignore clicks outside the viewport
  if ( X < vp[0] || X > vp[2] || Y < vp[1] || Y > vp[3]) return;


  if (ev->button() == Qt::LeftButton) {
    m_leftMouseDown = true;
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = true;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = true;
  }
}

void rtCameraControl::mouseMove(QMouseEvent* ev,int window) {
  QPoint newPt;
  newPt = ev->pos();
  if (m_leftMouseDown) {
    // Rotate the camera.
    m_camera->OrthogonalizeViewUp();
    m_camera->Elevation((double)(m_lastPoint.y() - newPt.y())/(-3.0f));
    m_camera->Azimuth((double)(m_lastPoint.x() - newPt.x())/3.0f);
    m_lastPoint = newPt;
    m_renderer->ResetCameraClippingRange();
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  } else if (m_rightMouseDown) {
    // Translate the camera
    double pos[3];
    double foc[3];
    double viewUp[3];
    double planeNorm[3];
    double xdirec[3];
    double yDiff = (double)(m_lastPoint.y() - newPt.y());
    double xDiff = (double)(m_lastPoint.x() - newPt.x());
    m_camera->GetPosition(pos);
    m_camera->GetFocalPoint(foc);
    m_camera->GetViewUp(viewUp);
    m_camera->GetViewPlaneNormal(planeNorm);

    xdirec[0] = viewUp[1]*planeNorm[2] - viewUp[2]*planeNorm[1];
    xdirec[1] = viewUp[2]*planeNorm[0] - viewUp[0]*planeNorm[2];
    xdirec[2] = viewUp[0]*planeNorm[1] - viewUp[1]*planeNorm[0];

    for (int ix1=0; ix1<3; ix1++) {
      pos[ix1] = pos[ix1] - viewUp[ix1]*yDiff + xdirec[ix1]*xDiff;
      foc[ix1] = foc[ix1] - viewUp[ix1]*yDiff + xdirec[ix1]*xDiff;
    }

    m_camera->SetPosition(pos);
    m_camera->SetFocalPoint(foc);
    m_lastPoint = newPt;
    m_renderer->ResetCameraClippingRange();
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void rtCameraControl::mouseRelease(QMouseEvent* ev,int window) {
  if (ev->button() == Qt::LeftButton) {
    m_leftMouseDown = false;
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = false;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = false;
  }

  m_renderer->ResetCameraClippingRange();
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

void rtCameraControl::mouseDoubleClick(QMouseEvent* ev,int window) {
}

void rtCameraControl::keyPress(QKeyEvent* ev,int window) {
  bool refresh = true;

  m_camera->OrthogonalizeViewUp();
  switch(ev->key()) {
    case (Qt::Key_Left):
    m_camera->Azimuth(1);
    break;
    case (Qt::Key_Right):
    m_camera->Azimuth(-1);
    break;
    case (Qt::Key_Up):
    m_camera->Elevation(-1);
    break;
    case (Qt::Key_Down):
    m_camera->Elevation(1);
    break;
  case (Qt::Key_PageUp):
    m_camera->Roll(1);
    break;
  case (Qt::Key_PageDown):
    m_camera->Roll(-1);
    break;
    default:
    refresh = false;
    break;
  }
  if (refresh) {
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void rtCameraControl::keyRelease(QKeyEvent* ev,int window) {
}

void rtCameraControl::wheel(QWheelEvent* ev,int window) {
  if (!ev) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Wheel event pointer is NULL."));
    return;
  }

  double vp[4];
  m_renderer->GetViewport(vp);

  // Convert the Qt x, y to VTK viewport.
  QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->size();
  double X = ((double)ev->x()/(double)winSize.width());
  double Y = ((double)(winSize.height()-ev->y())/(double)winSize.height());

  // Ignore wheel movements outside the viewport
  if ( X < vp[0] || X > vp[2] || Y < vp[1] || Y > vp[3]) return;


  m_scrollTimer.stop();
  m_scrollWheelMotion = true;
  connect(&m_scrollTimer, SIGNAL(timeout()), this, SLOT(wheelFinished()));
  m_scrollTimer.start();

  m_camera->Dolly(1.0+((double)ev->delta())/1200.0f);
  m_camera->OrthogonalizeViewUp();
  m_renderer->ResetCameraClippingRange();
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

void rtCameraControl::wheelFinished() {
  m_scrollWheelMotion = false;
  m_renderer->ResetCameraClippingRange();
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

/////////////////
// Protected Functions
/////////////////
void rtCameraControl::connectEvents() {
  if (!m_eventWidget) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not connect events. QVTK widget pointer is NULL."));
    return;
  }
  connect(m_eventWidget, SIGNAL(cameraMousePress(QMouseEvent*,int)), this, SLOT(mousePress(QMouseEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraMouseMove(QMouseEvent*,int)), this, SLOT(mouseMove(QMouseEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraMouseRelease(QMouseEvent*,int)), this, SLOT(mouseRelease(QMouseEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClick(QMouseEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraKeyPress(QKeyEvent*,int)), this, SLOT(keyPress(QKeyEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraKeyRelease(QKeyEvent*,int)), this, SLOT(keyRelease(QKeyEvent*,int)));
  connect(m_eventWidget, SIGNAL(cameraWheel(QWheelEvent*,int)), this, SLOT(wheel(QWheelEvent*,int)));
}
