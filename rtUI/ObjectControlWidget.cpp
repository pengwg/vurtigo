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

#include "ObjectControlWidget.h"

#include "rtMainWindow.h"
#include "rtCameraControl.h"
#include "rtApplication.h"

#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPropCollection.h>
#include <vtkPlane.h>
#include <vtkMath.h>
#include <vtkWorldPointPicker.h>

#include <algorithm>

ObjectControlWidget::ObjectControlWidget() {
  m_showing = false;
  m_xsize = 1.0;
  m_ysize = 1.0;
  m_transform = vtkTransform::New();
  m_transform->Identity();
  m_userTransform = vtkTransform::New();
  m_userTransform->Identity();

  // Setup how the points are located around the widget
  //////////////////////
  // 0 1 2
  // 3 4 5
  // 6 7 8
  ////////////////////
  for (int ix1=0; ix1<3; ix1++) {
    for (int ix2=0; ix2<3; ix2++) {
      m_pointLocations[ix1+3*ix2][0] = ix1;
      m_pointLocations[ix1+3*ix2][1] = ix2;
      m_pointLocations[ix1+3*ix2][2] = 0.0f;  // The z is centered.
    }
  }

  m_corners[0] = 0;
  m_corners[1] = 2;
  m_corners[2] = 8;
  m_corners[3] = 6;

  // Create the pipeline for the 3 rings
  for (int ix1=0; ix1<3; ix1++) {
    m_position[ix1] = vtkTransform::New();
    m_torus[ix1] = vtkParametricTorus::New();
    m_torusSrc[ix1] = vtkParametricFunctionSource::New();
    m_diskMapper[ix1] = vtkPolyDataMapper::New();
    m_diskActor[ix1] = vtkActor::New();

    m_torus[ix1]->SetRingRadius(70.0);
    m_torus[ix1]->SetCrossSectionRadius(2.0);

    m_torusSrc[ix1]->SetParametricFunction(m_torus[ix1]);
    m_diskMapper[ix1]->SetInputConnection(m_torusSrc[ix1]->GetOutputPort());
    m_diskActor[ix1]->SetMapper(m_diskMapper[ix1]);
  }

  // Create the pipeline for the point.
  m_touchPoint = vtkSphereSource::New();
  m_pointMapper = vtkPolyDataMapper::New();
  m_pointActor = vtkActor::New();

  m_touchPoint->SetCenter(m_pointLocations[4]);
  m_touchPoint->SetRadius(std::max(m_xsize, m_ysize)*0.1);

  m_pointMapper->SetInputConnection(m_touchPoint->GetOutputPort());
  m_pointActor->SetMapper(m_pointMapper);

  m_boxOutline.setOutlineColor(1.0f, 0.0f, 0.0f);
  m_boxOutline.setCorners(m_pointLocations[m_corners[0]], m_pointLocations[m_corners[1]], m_pointLocations[m_corners[2]], m_pointLocations[m_corners[3]]);

  m_crosshair.setColor(1.0f, 0.0f, 0.0f);
  m_crosshair.setCorners(m_pointLocations[m_corners[0]], m_pointLocations[m_corners[1]], m_pointLocations[m_corners[2]], m_pointLocations[m_corners[3]]);
  m_crosshair.setVisible(false);

  m_currProp = NULL;

  // Opacity starts off as 100% opaque.
  setWidgetOpacity(1.0);

  // No object of interest just yet.
  m_objOfInterest = NULL;

  connect( rtApplication::instance().getMainWinHandle(), SIGNAL(cameraModeSignal(bool)), this, SLOT(cameraMode(bool)) );
  connect( rtApplication::instance().getMainWinHandle(), SIGNAL(interactionModeSignal(bool)), this, SLOT(interactionMode(bool)) );
  connect( rtApplication::instance().getMainWinHandle(), SIGNAL(placeModeSignal(bool)), this, SLOT(placeMode(bool)) );
}

ObjectControlWidget::~ObjectControlWidget() {
  m_transform->Delete();
  m_userTransform->Delete();

  for (int ix1=0; ix1<3; ix1++) {
    m_position[ix1]->Delete();
    m_torus[ix1]->Delete();
    m_torusSrc[ix1]->Delete();
    m_diskMapper[ix1]->Delete();
    m_diskActor[ix1]->Delete();
  }


  m_touchPoint->Delete();
  m_pointMapper->Delete();
  m_pointActor->Delete();

}

void ObjectControlWidget::setTransform(vtkTransform* input) {
  if (!input || m_currProp) return;

  m_transform->DeepCopy(input);
  if(m_showing) {
    updateWidgetPosition();
  }
}

void ObjectControlWidget::getTransform(vtkTransform* output) {
  output->Identity();
  output->Concatenate(m_transform);
}

void ObjectControlWidget::setSize(double xsize, double ysize) {
  m_xsize = xsize/2.0f;
  m_ysize = ysize/2.0f;

  // When the size changes so do the point locations.
  for (int ix1=0; ix1<3; ix1++) {
    for (int ix2=0; ix2<3; ix2++) {
      m_pointLocations[ix1+3*ix2][0] = (ix1)*m_xsize;
      m_pointLocations[ix1+3*ix2][1] = (ix2)*m_ysize;
      m_pointLocations[ix1+3*ix2][2] = 0.0f;  // The z is centered.
    }
  }
}


double ObjectControlWidget::getXSize() {
  return m_xsize*2.0f;
}

double ObjectControlWidget::getYSize() {
  return m_ysize*2.0f;
}

void ObjectControlWidget::show() {
  if(m_showing) return;
  m_showing = true;

  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();
  updateWidgetPosition();

  ren->AddViewProp(m_pointActor);
  ren->AddViewProp(m_boxOutline.getActor());
  ren->AddViewProp(m_crosshair.getActor());

  for (int ix1=0; ix1<3; ix1++) {
    ren->AddViewProp(m_diskActor[ix1]);
  }
  // Make the window 'dirty' so that it is rerendered
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

void ObjectControlWidget::hide() {
  if(!m_showing) return;
  m_showing = false;

  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();

  ren->RemoveViewProp(m_pointActor);
  ren->RemoveViewProp(m_boxOutline.getActor());
  ren->RemoveViewProp(m_crosshair.getActor());

  for (int ix1=0; ix1<3; ix1++) {
    ren->RemoveViewProp(m_diskActor[ix1]);
  }
  // Make the window 'dirty' so that it is rerendered
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

bool ObjectControlWidget::isShowing() {
  return m_showing;
}

void ObjectControlWidget::setUserTransform(vtkTransform* t) {
  if (!t) return;

  m_boxOutline.setUserTransform(t);
  m_crosshair.setUserTransform(t);
  m_pointActor->SetUserTransform(t);
  m_userTransform->Identity();
  m_userTransform->Concatenate(t);
}

void ObjectControlWidget::setWidgetOpacity(double op) {
  if (op > 1.0) op = 1.0;
  else if (op < 0.0) op = 0.0;

  m_widgetOpacity = op;
  m_pointActor->GetProperty()->SetOpacity(op);
  m_diskActor[0]->GetProperty()->SetOpacity(op);
  m_diskActor[1]->GetProperty()->SetOpacity(op);
  m_diskActor[2]->GetProperty()->SetOpacity(op);
}

double ObjectControlWidget::getWidgetOpacity() {
  return m_widgetOpacity;
}

/////////////////
// Public Slots
/////////////////
void ObjectControlWidget::mousePressEvent(QMouseEvent* event) {
  if(!m_showing) return;

  if (event->button() == Qt::LeftButton && !m_currProp) {
    QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget()->size();
    int X = event->x();
    int Y = winSize.height()-event->y();

    m_oldX = X;
    m_oldY = Y;

    // Find the current prop and the position in 3D of the click.
    m_currProp = getLocalPropAt(X, Y, m_clickPosition);

    if ( m_currProp ) {
      // If the point is selected...
      if (m_currProp == m_pointActor) {
        m_pointActor->VisibilityOff();
        m_crosshair.setVisible(true);
        setWidgetOpacity(0.1);
      }

      for (int ix1=0; ix1<3; ix1++) {

        // Check each rotating disk to see if one has been picked.
        if (m_currProp == m_diskActor[ix1]) {
          double pos2[3];

          setWidgetOpacity(0.1);
          m_diskActor[ix1]->GetProperty()->SetOpacity(1.0);

          m_position[ix1]->Inverse();
          m_position[ix1]->TransformPoint(m_clickPosition, pos2);
          m_position[ix1]->Inverse();

          if (pos2[0] > 0 && pos2[1] > 0) {
            m_positiveDirection[0] = pos2[0]+50.0;
            m_positiveDirection[1] = pos2[1]-50.0;
          } else if (pos2[0] > 0 && pos2[1] <= 0) {
            m_positiveDirection[0] = pos2[0]-50.0;
            m_positiveDirection[1] = pos2[1]-50.0;
          } else if (pos2[0] <= 0 && pos2[1] <= 0) {
            m_positiveDirection[0] = pos2[0]-50.0;
            m_positiveDirection[1] = pos2[1]+50.0;
          } else {
            m_positiveDirection[0] = pos2[0]+50.0;
            m_positiveDirection[1] = pos2[1]+50.0;
          }
          m_positiveDirection[2] = pos2[2]+0.0f;
          m_position[ix1]->TransformPoint(m_positiveDirection, m_positiveDirectionT);
        }
      }
    }

    // Change the prop color to red to show it is activated.
    if (m_currProp) {
      m_currProp->GetProperty()->SetColor(1.0, 0.0, 0.0);
    }

    // Make the window 'dirty' so that it is rerendered
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void ObjectControlWidget::mouseMoveEvent(QMouseEvent* event) {
  if(!m_showing || !event) return;

  if(event->buttons() == Qt::NoButton && !m_currProp) {
    QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget()->size();
    int X = event->x();
    int Y = winSize.height()-event->y();
    double pos3D[3];
    vtkActor* tempActor;

    // Find the current prop and the position in 3D of the click.
    tempActor = getLocalPropAt(X, Y, pos3D);

    if (tempActor) {
      setWidgetOpacity(1.0);
    } else if ( pickedObjectOfInterest(X, Y) ) {
      setWidgetOpacity(0.3);
    } else {
      setWidgetOpacity(0.01);
    }

    // Make the window 'dirty' so that it is rerendered
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);

    return;
  } else if (!m_currProp) {
    return;
  }

  QSize winSize = rtApplication::instance().getMainWinHandle()->getRenderWidget()->size();
  int X = event->x();
  int Y = winSize.height()-event->y();

  double cameraRight[3];
  double cameraUp[3];
  double cameraForward[3];
  double normalDirectionT[3];

  rtApplication::instance().getMainWinHandle()->getCameraRight(cameraRight);
  rtApplication::instance().getMainWinHandle()->getCameraUp(cameraUp);
  rtApplication::instance().getMainWinHandle()->getCameraForward(cameraForward);

  normalDirectionT[0] = m_positiveDirectionT[0]-m_clickPosition[0];
  normalDirectionT[1] = m_positiveDirectionT[1]-m_clickPosition[1];
  normalDirectionT[2] = m_positiveDirectionT[2]-m_clickPosition[2];
  vtkMath::Normalize(normalDirectionT);

  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }

  double desiredPoint[3];

  desiredPoint[0] = X;
  desiredPoint[1] = Y;

  double viewZ[3];
  for (int ix1=0; ix1<3; ix1++) viewZ[ix1] = m_convertedLocations[4][ix1];
  m_userTransform->TransformPoint(viewZ, viewZ);
  rtApplication::instance().getMainWinHandle()->getRenderer()->WorldToView(viewZ[0], viewZ[1], viewZ[2]);
  desiredPoint[2] = viewZ[2];

  rtApplication::instance().getMainWinHandle()->getRenderer()->DisplayToNormalizedDisplay(desiredPoint[0], desiredPoint[1]);
  rtApplication::instance().getMainWinHandle()->getRenderer()->NormalizedDisplayToViewport(desiredPoint[0], desiredPoint[1]);
  rtApplication::instance().getMainWinHandle()->getRenderer()->ViewportToNormalizedViewport(desiredPoint[0], desiredPoint[1]);
  rtApplication::instance().getMainWinHandle()->getRenderer()->NormalizedViewportToView(desiredPoint[0], desiredPoint[1], desiredPoint[2]);
  rtApplication::instance().getMainWinHandle()->getRenderer()->ViewToWorld(desiredPoint[0], desiredPoint[1], desiredPoint[2]);

  m_userTransform->Inverse();
  m_userTransform->TransformPoint(desiredPoint, desiredPoint);
  m_userTransform->Inverse();

  for (int ix1=0; ix1<3; ix1++) {
    desiredPoint[ix1] = desiredPoint[ix1] - m_convertedLocations[4][ix1] + m_convertedLocations[0][ix1];
  }

  vtkTransform *movement = vtkTransform::New();
  movement->Identity();

  if (m_currProp == m_pointActor) {
    double pos[3];

    m_transform->PostMultiply();
    m_transform->GetPosition(pos);
    m_transform->Translate(-pos[0], -pos[1], -pos[2]);
    m_transform->Translate(desiredPoint);
    m_transform->PreMultiply();

    // Re-render
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  } else {
    vtkTransform* viewTransform = vtkTransform::New();
    vtkMatrix4x4* tempMatrix = vtkMatrix4x4::New();
    double mouseDirec[3];
    double viewDirec[3];

    mouseDirec[0] = (X-m_oldX);
    mouseDirec[1] = (Y-m_oldY);
    mouseDirec[2] = 0.0f;

    tempMatrix->DeepCopy(rtApplication::instance().getMainWinHandle()->getCameraControl()->getViewMatrix());
    tempMatrix->SetElement(0, 3, 0.0f);
    tempMatrix->SetElement(1, 3, 0.0f);
    tempMatrix->SetElement(2, 3, 0.0f);
    viewTransform->SetMatrix(tempMatrix);
    viewTransform->Inverse();
    viewTransform->TransformPoint(mouseDirec, viewDirec);
    vtkMath::Normalize(viewDirec);
    viewTransform->Delete();
    tempMatrix->Delete();

    double dotProd = normalDirectionT[0]*viewDirec[0]+normalDirectionT[1]*viewDirec[1]+normalDirectionT[2]*viewDirec[2];
    double rotate=dotProd*3;

    if (rotate < 1.0 && rotate >= 0) {
      rotate = -1.0f;
    } else if (rotate > -1.0 && rotate <= 0) {
      rotate = 1.0f;
    }

    if (m_currProp == m_diskActor[0]) {
      double rotateAxis[3];
      for (int ix1=0; ix1<3 ;ix1++) {
        rotateAxis[ix1] = m_convertedLocations[7][ix1] - m_convertedLocations[4][ix1];
      }
      movement->PostMultiply();
      movement->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      movement->RotateWXYZ(rotate, rotateAxis[0], rotateAxis[1], rotateAxis[2]);
      movement->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      movement->PreMultiply();

      rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
    } else if (m_currProp == m_diskActor[1]) {
      double rotateAxis[3];
      for (int ix1=0; ix1<3 ;ix1++) {
        rotateAxis[ix1] = m_convertedLocations[3][ix1] - m_convertedLocations[4][ix1];
      }
      movement->PostMultiply();
      movement->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      movement->RotateWXYZ(rotate, rotateAxis[0], rotateAxis[1], rotateAxis[2]);
      movement->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      movement->PreMultiply();

      rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
    } else if (m_currProp == m_diskActor[2]) {
      double planeNormal[3];

      for (int ix1=0; ix1<3; ix1++) {
        planeNormal[ix1] = m_transform->GetMatrix()->GetElement(ix1, 2);
      }

      movement->PostMultiply();
      movement->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      movement->RotateWXYZ(rotate, -planeNormal[0], -planeNormal[1], -planeNormal[2]);
      movement->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      movement->PreMultiply();

      rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
    }
  }

  m_transform->PostMultiply();
  m_transform->Concatenate(movement);
  m_transform->PreMultiply();

  movement->Delete();

  updateWidgetPosition();
  m_oldX = X;
  m_oldY = Y;
}

void ObjectControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing || !m_currProp) return;
  if (event->button() == Qt::LeftButton) {
    // Clear the color
    m_currProp->GetProperty()->SetColor(1.0f, 1.0f, 1.0f);

    // If the point is selected...
    if (m_currProp == m_pointActor) {
      m_pointActor->VisibilityOn();
      m_crosshair.setVisible(false);    
    }
    setWidgetOpacity(1.0);

    m_currProp = NULL;

    // Make the window 'dirty' so that it is rerendered
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void ObjectControlWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::keyPressEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::keyReleaseEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::wheelEvent(QWheelEvent* event) {
  if(!m_showing) return;
  int numSteps = event->delta() / 32;
  double increment = 1.0;

  if (event->modifiers() & Qt::ShiftModifier)  increment = 0.1;

  vtkTransform *truePos = vtkTransform::New();
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  truePos->Identity();
  truePos->Concatenate(m_transform);
  truePos->GetMatrix(mat);

  double zDirec[3];
  double sumSq = 0.0;
  for (int ix1=0; ix1<3; ix1++) {
    zDirec[ix1] = mat->GetElement(ix1, 2);
    sumSq += zDirec[ix1]*zDirec[ix1];
  }
  sumSq = sqrt(sumSq);

  double cameraDirec[3];
  double dotP=0.0f;
  rtApplication::instance().getMainWinHandle()->getCameraForward(cameraDirec);

  m_userTransform->MultiplyPoint(cameraDirec, cameraDirec);

  dotP = zDirec[0]*cameraDirec[0]+zDirec[1]*cameraDirec[1]+zDirec[2]*cameraDirec[2];
  if (dotP > 0) {
    dotP = 1.0f;
  } else if (dotP < 0) {
    dotP = -1.0f;
  }

  for (int ix1=0; ix1<3; ix1++) {
    mat->SetElement( ix1, 3, mat->GetElement(ix1, 3) + (increment*dotP*numSteps*zDirec[ix1]/sumSq) );
  }
  m_transform->SetMatrix(mat);

  mat->Delete();
  truePos->Delete();
  updateWidgetPosition();
}

void ObjectControlWidget::cameraMode(bool toggle) {
  if (toggle) hide();
}

void ObjectControlWidget::interactionMode(bool toggle) {
  if(!toggle) hide();
}

void ObjectControlWidget::placeMode(bool toggle) {
  if(toggle) hide();
}


//////////////
// Protected
//////////////

void ObjectControlWidget::updateWidgetPosition() {
  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }

  // Put it in the middle of the plane.
  m_touchPoint->SetCenter(m_convertedLocations[4]);
  m_touchPoint->SetRadius(std::max(m_xsize, m_ysize)*0.05);

  m_boxOutline.setCorners(m_convertedLocations[m_corners[0]], m_convertedLocations[m_corners[1]], m_convertedLocations[m_corners[2]], m_convertedLocations[m_corners[3]]);
  m_crosshair.setCorners(m_convertedLocations[m_corners[0]], m_convertedLocations[m_corners[1]], m_convertedLocations[m_corners[2]], m_convertedLocations[m_corners[3]]);

  double pos[3];
  m_transform->GetPosition(pos);
  for (int ix1=0; ix1<3; ix1++) {

    // Update Size too.
    m_torus[ix1]->SetRingRadius(std::max(m_xsize, m_ysize)*0.75);
    m_torus[ix1]->SetCrossSectionRadius(std::max(m_xsize, m_ysize)*0.03);

    m_position[ix1]->Identity();
    m_position[ix1]->Concatenate(m_userTransform);
    m_position[ix1]->Translate(m_convertedLocations[4][0] - pos[0], m_convertedLocations[4][1] - pos[1], m_convertedLocations[4][2] - pos[2]);
    m_position[ix1]->Concatenate(m_transform);

  }

  m_position[0]->RotateX(90.0f);
  m_position[1]->RotateY(90.0f);
  m_position[2]->RotateZ(90.0f);

  for (int ix1=0; ix1<3; ix1++) {
    m_diskActor[ix1]->SetUserTransform(m_position[ix1]);
  }
}

vtkActor* ObjectControlWidget::getLocalPropAt(int x, int y, double clickPos[3]) {
  vtkActor* result;
  vtkPropCollection* col = vtkPropCollection::New();
  vtkPropPicker* pick = vtkPropPicker::New();
  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();

  col->AddItem(m_pointActor);
  col->AddItem(m_diskActor[0]);
  col->AddItem(m_diskActor[1]);
  col->AddItem(m_diskActor[2]);

  result = NULL;
  if (pick->PickProp(x, y, ren, col) ) {
      result = static_cast<vtkActor*>(pick->GetViewProp());
      pick->GetPickPosition(clickPos);
  }

  if(col) col->Delete();
  if(pick) pick->Delete();

  return result;
}

bool ObjectControlWidget::pickedObjectOfInterest(int x, int y) {
  if (!m_objOfInterest) return false;

  vtkActor* result;
  vtkPropCollection* col = vtkPropCollection::New();
  vtkPropPicker* pick = vtkPropPicker::New();
  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();

  col->AddItem(m_objOfInterest);

  result = NULL;
  if (pick->PickProp(x, y, ren, col) ) {
      result = static_cast<vtkActor*>(pick->GetViewProp());
  }

  if(col) col->Delete();
  if(pick) pick->Delete();

  return result == m_objOfInterest;
}
