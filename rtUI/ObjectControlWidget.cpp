#include "ObjectControlWidget.h"

#include "rtObjectManager.h"
#include "rtMainWindow.h"
#include "rtCameraControl.h"

#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPropCollection.h>
#include <vtkPlane.h>
#include <vtkMath.h>

#include <algorithm>

ObjectControlWidget::ObjectControlWidget() {
  m_showing = false;
  m_xsize = 1.0;
  m_ysize = 1.0;
  m_transform = vtkTransform::New();
  m_transform->Identity();

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

    m_torus[ix1]->SetRingRadius(75);
    m_torus[ix1]->SetCrossSectionRadius(5);

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

  m_currProp = NULL;
}

ObjectControlWidget::~ObjectControlWidget() {
  m_transform->Delete();

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
  output->DeepCopy(m_transform);
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

  vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();
  updateWidgetPosition();

  ren->AddViewProp(m_pointActor);


  ren->AddViewProp(m_boxOutline.getActor());


  for (int ix1=0; ix1<3; ix1++) {
    ren->AddViewProp(m_diskActor[ix1]);
  }

}

void ObjectControlWidget::hide() {
  if(!m_showing) return;
  m_showing = false;

  vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();

  ren->RemoveViewProp(m_pointActor);

  ren->RemoveViewProp(m_boxOutline.getActor());

  for (int ix1=0; ix1<3; ix1++) {
    ren->RemoveViewProp(m_diskActor[ix1]);
  }
}

bool ObjectControlWidget::isShowing() {
  return m_showing;
}

void ObjectControlWidget::mousePressEvent(QMouseEvent* event) {
  if(!m_showing) return;

  if (event->button() == Qt::LeftButton && !m_currProp) {
    QSize winSize = rtObjectManager::instance().getMainWinHandle()->getRenderWidget()->size();
    int X = event->x();
    int Y = winSize.height()-event->y();

    m_oldX = X;
    m_oldY = Y;

    vtkPropCollection* col = vtkPropCollection::New();
    vtkPropPicker* pick = vtkPropPicker::New();
    vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();
    
    col->AddItem(m_pointActor);
    col->AddItem(m_diskActor[0]);
    col->AddItem(m_diskActor[1]);
    col->AddItem(m_diskActor[2]);

    if (pick->PickProp(X, Y, ren, col) ) {
      m_currProp = static_cast<vtkActor*>(pick->GetViewProp());
      double pos2[3];
      pick->GetPickPosition(m_clickPosition);

      for (int ix1=0; ix1<3; ix1++) {
        if (m_currProp == m_diskActor[ix1]) {
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
    if (m_currProp) {
      m_currProp->GetProperty()->SetColor(1.0, 0.0, 0.0);
    }
  }
}

void ObjectControlWidget::mouseMoveEvent(QMouseEvent* event) {
  if(!m_showing || !event || !m_currProp) return;

  QSize winSize = rtObjectManager::instance().getMainWinHandle()->getRenderWidget()->size();
  int X = event->x();
  int Y = winSize.height()-event->y();

  double cameraRight[3];
  double cameraUp[3];
  double cameraForward[3];
  double temp[3];
  double dist = rtObjectManager::instance().getMainWinHandle()->getCameraDistance();
  double xdiff = (X-m_oldX)*(dist)/1200.0f;
  double ydiff = (Y-m_oldY)*(dist)/1200.0f;



  double planeNormal[3];
  for (int ix1=0; ix1<3; ix1++) {
    planeNormal[ix1] = m_transform->GetMatrix()->GetElement(ix1, 2);
  }

  rtObjectManager::instance().getMainWinHandle()->getCameraRight(cameraRight);
  rtObjectManager::instance().getMainWinHandle()->getCameraUp(cameraUp);
  rtObjectManager::instance().getMainWinHandle()->getCameraForward(cameraForward);

  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }


  if (m_currProp == m_pointActor) {
    temp[0] = cameraRight[0]*xdiff+cameraUp[0]*ydiff;
    temp[1] = cameraRight[1]*xdiff+cameraUp[1]*ydiff;
    temp[2] = cameraRight[2]*xdiff+cameraUp[2]*ydiff;

    m_transform->PostMultiply();
    m_transform->Translate(temp[0], temp[1], temp[2]);
    m_transform->PreMultiply();
  } else {

    double normalDirectionT[3];
    vtkTransform* viewTransform = vtkTransform::New();
    vtkMatrix4x4* tempMatrix = vtkMatrix4x4::New();

    normalDirectionT[0] = m_positiveDirectionT[0]-m_clickPosition[0];
    normalDirectionT[1] = m_positiveDirectionT[1]-m_clickPosition[1];
    normalDirectionT[2] = m_positiveDirectionT[2]-m_clickPosition[2];
    vtkMath::Normalize(normalDirectionT);

    tempMatrix->DeepCopy(rtObjectManager::instance().getMainWinHandle()->getCameraControl()->getViewMatrix());

    tempMatrix->SetElement(0, 3, 0.0f);
    tempMatrix->SetElement(1, 3, 0.0f);
    tempMatrix->SetElement(2, 3, 0.0f);
    viewTransform->SetMatrix(tempMatrix);
    double mouseDirec[3];
    double viewDirec[3];
    double matrixP[3];

    mouseDirec[0] = (X-m_oldX);
    mouseDirec[1] = (Y-m_oldY);
    mouseDirec[2] = 0.0f;

    viewTransform->Inverse();
    viewTransform->TransformPoint(mouseDirec, viewDirec);
    viewTransform->Delete();
    tempMatrix->Delete();

    vtkMath::Normalize(viewDirec);

    double dotProd = normalDirectionT[0]*viewDirec[0]+normalDirectionT[1]*viewDirec[1]+normalDirectionT[2]*viewDirec[2];
    double rotate=dotProd*3;

    if (rotate < 1.0 && rotate >= 0) {
      rotate = 1.0f;
    } else if (rotate > -1.0 && rotate <= 0) {
      rotate = -1.0f;
    }

    if (m_currProp == m_diskActor[0]) {
      for (int ix1=0; ix1<3 ;ix1++) {
        temp[ix1] = m_convertedLocations[7][ix1] - m_convertedLocations[4][ix1];
      }
      m_transform->PostMultiply();
      m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
      m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      m_transform->PreMultiply();
    } else if (m_currProp == m_diskActor[1]) {
      for (int ix1=0; ix1<3 ;ix1++) {
        temp[ix1] = m_convertedLocations[3][ix1] - m_convertedLocations[4][ix1];
      }
      m_transform->PostMultiply();
      m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
      m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      m_transform->PreMultiply();
    } else if (m_currProp == m_diskActor[2]) {
      m_transform->PostMultiply();
      m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
      m_transform->RotateWXYZ(rotate, -planeNormal[0], -planeNormal[1], -planeNormal[2]);
      m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
      m_transform->PreMultiply();
    }
  }

  updateWidgetPosition();
  m_oldX = X;
  m_oldY = Y;
}

void ObjectControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing || !m_currProp) return;
  if (event->button() == Qt::LeftButton) {
    // Clear the color
    m_currProp->GetProperty()->SetColor(1.0f, 1.0f, 1.0f);
    m_currProp = NULL;
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
  int numSteps = event->delta() / 16;

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_transform->GetMatrix(mat);

  double zDirec[3];
  double sumSq = 0.0;
  for (int ix1=0; ix1<3; ix1++) {
    zDirec[ix1] = mat->GetElement(ix1, 2);
    sumSq += zDirec[ix1]*zDirec[ix1];
  }
  sumSq = sqrt(sumSq);

  double cameraDirec[3];
  double dotP=0.0f;
  rtObjectManager::instance().getMainWinHandle()->getCameraForward(cameraDirec);
  dotP = zDirec[0]*cameraDirec[0]+zDirec[1]*cameraDirec[1]+zDirec[2]*cameraDirec[2];
  if (dotP > 0) {
    dotP = 1.0f;
  } else if (dotP < 0) {
    dotP = -1.0f;
  }

  for (int ix1=0; ix1<3; ix1++) {
    mat->SetElement( ix1, 3, mat->GetElement(ix1, 3) + (dotP*numSteps*zDirec[ix1]/sumSq) );
  }
  m_transform->SetMatrix(mat);

  mat->Delete();
  updateWidgetPosition();
}

void ObjectControlWidget::updateWidgetPosition() {

  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }

  // Put it in the middle of the plane.
  m_touchPoint->SetCenter(m_convertedLocations[4]);
  m_touchPoint->SetRadius(std::max(m_xsize, m_ysize)*0.1);

  m_boxOutline.setCorners(m_convertedLocations[m_corners[0]], m_convertedLocations[m_corners[1]], m_convertedLocations[m_corners[2]], m_convertedLocations[m_corners[3]]);

  double pos[3];
  m_transform->GetPosition(pos);
  for (int ix1=0; ix1<3; ix1++) {

    // Update Size too.
    m_torus[ix1]->SetRingRadius(std::max(m_xsize, m_ysize)*0.75);
    m_torus[ix1]->SetCrossSectionRadius(std::max(m_xsize, m_ysize)*0.05);

    m_position[ix1]->Identity();
    m_position[ix1]->Translate(m_convertedLocations[4][0] - pos[0], m_convertedLocations[4][1] - pos[1], m_convertedLocations[4][2] - pos[2]);
    m_position[ix1]->Concatenate(m_transform->GetMatrix());

  }

  m_position[0]->RotateX(90.0f);
  m_position[1]->RotateY(90.0f);
  m_position[2]->RotateZ(90.0f);
  for (int ix1=0; ix1<3; ix1++) {
    m_diskActor[ix1]->SetUserTransform(m_position[ix1]);
  }
}
