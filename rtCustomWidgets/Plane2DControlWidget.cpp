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

#include <vtkRenderer.h>
#include <vtkMath.h>

#include "Plane2DControlWidget.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "rtCameraControl.h"

Plane2DControlWidget::Plane2DControlWidget()
{
  m_boxOutline.setColor(1.0, 0.0, 0.0);
  m_crosshair.setColor(1.0, 0.0, 0.0);
  m_crosshair.setVisible(false);

  updateWidgetPosition();
}

Plane2DControlWidget::~Plane2DControlWidget() {

}

void Plane2DControlWidget::sizeChanged() {
  updateWidgetPosition();
}

void Plane2DControlWidget::visibilityChanged() {
  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();

  if (m_showing) {
    // Widget is showing

    updateWidgetPosition();

    ren->AddViewProp(m_centralSphere.getActor());
    ren->AddViewProp(m_boxOutline.getActor());
    ren->AddViewProp(m_crosshair.getActor());

    for (int ix1=0; ix1<3; ix1++) {
      ren->AddViewProp(m_compassWidget.getActor(ix1));
    }
  } else {
    // Widget is not showing

    ren->RemoveViewProp(m_centralSphere.getActor());
    ren->RemoveViewProp(m_boxOutline.getActor());
    ren->RemoveViewProp(m_crosshair.getActor());

    for (int ix1=0; ix1<3; ix1++) {
      ren->RemoveViewProp(m_compassWidget.getActor(ix1));
    }
  }

  // Make the window 'dirty' so that it is rerendered
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}


void Plane2DControlWidget::userTransformChanged() {
  m_boxOutline.setUserTransform(m_userTransform);
  m_crosshair.setUserTransform(m_userTransform);
  m_centralSphere.getActor()->SetUserTransform(m_userTransform);
}

void Plane2DControlWidget::widgetOpacityChanged() {
  m_centralSphere.getPropertyHandle()->SetOpacity(m_widgetOpacity);
  m_compassWidget.getActor(0)->GetProperty()->SetOpacity(m_widgetOpacity);
  m_compassWidget.getActor(1)->GetProperty()->SetOpacity(m_widgetOpacity);
  m_compassWidget.getActor(2)->GetProperty()->SetOpacity(m_widgetOpacity);
}

///////////////////
// Public Slots
///////////////////
void Plane2DControlWidget::mousePressEvent(QMouseEvent* event) {
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
      if (m_currProp == m_centralSphere.getActor()) {
        m_centralSphere.getActor()->VisibilityOff();
        m_crosshair.setVisible(true);
        setWidgetOpacity(0.1);
      }

      for (int ix1=0; ix1<3; ix1++) {
        // Check each rotating disk to see if one has been picked.
        if (m_currProp == m_compassWidget.getActor(ix1)) {
          setWidgetOpacity(0.1);
          m_compassWidget.getPropertyHandle(ix1)->SetOpacity(1.0);
          m_compassWidget.getPositiveRotationDirection(ix1, m_clickPosition, m_positiveDirection);
        }
      }
    }

    // Change the prop color to red to show it is activated.
    if (m_currProp) {
      m_currProp->GetProperty()->SetColor(1.0, 0.0, 0.0);
    }
    // we clicked on thin air
    else
    {
        rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMousePress(event);
    }

    // Make the window 'dirty' so that it is rerendered
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void Plane2DControlWidget::mouseMoveEvent(QMouseEvent* event) {
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
      rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMouseMove(event);
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

  normalDirectionT[0] = m_positiveDirection[0]-m_clickPosition[0];
  normalDirectionT[1] = m_positiveDirection[1]-m_clickPosition[1];
  normalDirectionT[2] = m_positiveDirection[2]-m_clickPosition[2];
  vtkMath::Normalize(normalDirectionT);

  updateConvertedLocations();

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

  if (m_currProp == m_centralSphere.getActor()) {
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

    if (m_currProp == m_compassWidget.getActor(0)) {
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
    } else if (m_currProp == m_compassWidget.getActor(1)) {
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
    } else if (m_currProp == m_compassWidget.getActor(2)) {
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

void Plane2DControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing || !m_currProp)
    {
      // we released over nothing
      if (m_showing) rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMouseRelease(event);
      return;
  }


  if (event->button() == Qt::LeftButton) {
    // Clear the color
    m_currProp->GetProperty()->SetColor(1.0f, 1.0f, 1.0f);

    // If the point is selected...
    if (m_currProp == m_centralSphere.getActor()) {
      m_centralSphere.getActor()->VisibilityOn();
      m_crosshair.setVisible(false);
    }
    setWidgetOpacity(1.0);

    m_currProp = NULL;

    // Make the window 'dirty' so that it is rerendered
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void Plane2DControlWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void Plane2DControlWidget::keyPressEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void Plane2DControlWidget::keyReleaseEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void Plane2DControlWidget::wheelEvent(QWheelEvent* event) {
  if(!m_showing) return;
  double numSteps = event->delta() / 120; // Changed by Ethan 2011-01-04 (maybe we want 12 for 1 cm?)
  double increment = 1.0;

  // Make a smaller increment if the user is holding down shift.
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
  rtApplication::instance().getMainWinHandle()->getCameraForward(cameraDirec);

  m_userTransform->MultiplyPoint(cameraDirec, cameraDirec);

  double dotP = vtkMath::Dot(zDirec, cameraDirec);
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


///////////////////
// Protected
//////////////////
void Plane2DControlWidget::updateWidgetPosition() {
  updateConvertedLocations();

  // Put it in the middle of the plane.
  m_centralSphere.setPosition(m_convertedLocations[4]);
  m_centralSphere.setRadius(std::max(m_xsize, m_ysize)*0.025);

  m_boxOutline.setCorners(m_convertedLocations[m_corners[0]], m_convertedLocations[m_corners[1]], m_convertedLocations[m_corners[2]], m_convertedLocations[m_corners[3]]);
  m_crosshair.setCorners(m_convertedLocations[m_corners[0]], m_convertedLocations[m_corners[1]], m_convertedLocations[m_corners[2]], m_convertedLocations[m_corners[3]]);

  double pos[3];
  m_transform->GetPosition(pos);
  for (int ix1=0; ix1<3; ix1++) {

    // Update Size too.
    m_compassWidget.setRadius(ix1, std::max(m_xsize, m_ysize)*0.375);
    m_compassWidget.setCrossSectionRadius(ix1, std::max(m_xsize, m_ysize)*0.015);

    m_compassWidget.getPositionTransform(ix1)->Identity();
    m_compassWidget.getPositionTransform(ix1)->Concatenate(m_userTransform);
    m_compassWidget.getPositionTransform(ix1)->Translate(m_convertedLocations[4][0] - pos[0], m_convertedLocations[4][1] - pos[1], m_convertedLocations[4][2] - pos[2]);
    m_compassWidget.getPositionTransform(ix1)->Concatenate(m_transform);

  }

  m_compassWidget.getPositionTransform(0)->RotateX(90.0f);
  m_compassWidget.getPositionTransform(1)->RotateY(90.0f);
  m_compassWidget.getPositionTransform(2)->RotateZ(90.0f);

  m_compassWidget.applyPositionTransform();
}

vtkActor* Plane2DControlWidget::getLocalPropAt(int x, int y, double clickPos[3]) {
  vtkActor* result;
  vtkPropCollection* col = vtkPropCollection::New();
  vtkPropPicker* pick = vtkPropPicker::New();
  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();

  col->AddItem(m_centralSphere.getActor());
  col->AddItem(m_compassWidget.getActor(0));
  col->AddItem(m_compassWidget.getActor(1));
  col->AddItem(m_compassWidget.getActor(2));

  result = NULL;
  if (pick->PickProp(x, y, ren, col) ) {
      result = static_cast<vtkActor*>(pick->GetViewProp());
      pick->GetPickPosition(clickPos);
  }

  if(col) col->Delete();
  if(pick) pick->Delete();

  return result;
}
