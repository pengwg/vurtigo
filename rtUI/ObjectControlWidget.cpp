#include "ObjectControlWidget.h"

#include "rtObjectManager.h"
#include "rtMainWindow.h"

#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPropCollection.h>

ObjectControlWidget::ObjectControlWidget() {
  m_showing = false;
  m_size = 1.0;
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

  // Create the pipeline.
  for (int ix1=0; ix1<9; ix1++) {
    m_touchPoints[ix1] = vtkSphereSource::New();
    m_pointMapper[ix1] = vtkPolyDataMapper::New();
    m_pointActor[ix1] = vtkActor::New();

    m_touchPoints[ix1]->SetCenter(m_pointLocations[ix1]);
    m_touchPoints[ix1]->SetRadius(m_size*0.1);

    m_pointMapper[ix1]->SetInputConnection(m_touchPoints[ix1]->GetOutputPort());
    m_pointActor[ix1]->SetMapper(m_pointMapper[ix1]);
  }

  for (int ix1=0; ix1<4; ix1++) {
    m_edges[ix1] = vtkLineSource::New();
    m_lineMapper[ix1] = vtkPolyDataMapper::New();
    m_lineActor[ix1] = vtkActor::New();

    m_edges[ix1]->SetPoint1(m_pointLocations[m_corners[ix1]][0], m_pointLocations[m_corners[ix1]][1], m_pointLocations[m_corners[ix1]][2]);
    m_edges[ix1]->SetPoint2(m_pointLocations[m_corners[(ix1+1)%4]][0], m_pointLocations[m_corners[(ix1+1)%4]][1], m_pointLocations[m_corners[(ix1+1)%4]][2]);
    m_edges[ix1]->SetResolution(1);

    m_lineMapper[ix1]->SetInputConnection(m_edges[ix1]->GetOutputPort());
    m_lineActor[ix1]->SetMapper(m_lineMapper[ix1]);
    m_lineActor[ix1]->GetProperty()->SetColor(1.0f, 0.0f, 0.0f);
  }
  m_currPropIndex = -1;
}

ObjectControlWidget::~ObjectControlWidget() {
  m_transform->Delete();

  for (int ix1=0; ix1<9; ix1++) {
    m_touchPoints[ix1]->Delete();
    m_pointMapper[ix1]->Delete();
    m_pointActor[ix1]->Delete();
  }

  for (int ix1=0; ix1<4; ix1++) {
    m_edges[ix1]->Delete();
    m_lineMapper[ix1]->Delete();
    m_lineActor[ix1]->Delete();
  }

}

void ObjectControlWidget::setTransform(vtkTransform* input) {
  if (!input || m_currPropIndex != -1) return;

  m_transform->DeepCopy(input);
  if(m_showing) {
    updateWidgetPosition();
  }
}

void ObjectControlWidget::getTransform(vtkTransform* output) {
  output->DeepCopy(m_transform);
}

void ObjectControlWidget::setSize(double size) {
  m_size = size/2.0f;

  // When the size changes so do the point locations.
  for (int ix1=0; ix1<3; ix1++) {
    for (int ix2=0; ix2<3; ix2++) {
      m_pointLocations[ix1+3*ix2][0] = (ix1)*m_size;
      m_pointLocations[ix1+3*ix2][1] = (ix2)*m_size;
      m_pointLocations[ix1+3*ix2][2] = 0.0f;  // The z is centered.
    }
  }
}


double ObjectControlWidget::getSize() {
  return m_size;
}

void ObjectControlWidget::show() {
  if(m_showing) return;
  m_showing = true;

  vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();
  updateWidgetPosition();
  for (int ix1=0; ix1<9; ix1++) {
    ren->AddViewProp(m_pointActor[ix1]);
  }
  for (int ix1=0; ix1<4; ix1++) {
    ren->AddViewProp(m_lineActor[ix1]);
  }

}

void ObjectControlWidget::hide() {
  if(!m_showing) return;
  m_showing = false;

  vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();
  for (int ix1=0; ix1<9; ix1++) {
    ren->RemoveViewProp(m_pointActor[ix1]);
  }
  for (int ix1=0; ix1<4; ix1++) {
    ren->RemoveViewProp(m_lineActor[ix1]);
  }
}

bool ObjectControlWidget::isShowing() {
  return m_showing;
}

void ObjectControlWidget::mousePressEvent(QMouseEvent* event) {
  if(!m_showing) return;

  if (event->button() == Qt::LeftButton) {
    QSize winSize = rtObjectManager::instance().getMainWinHandle()->getRenderWidget()->size();
    int X = event->x();
    int Y = winSize.height()-event->y();

    m_oldX = X;
    m_oldY = Y;

    vtkPropCollection* col = vtkPropCollection::New();
    vtkPropPicker* pick = vtkPropPicker::New();
    vtkRenderer* ren = rtObjectManager::instance().getMainWinHandle()->getRenderer();
    for (int ix1=0; ix1<9; ix1++) {
      col->AddItem(m_pointActor[ix1]);
    }
    vtkProp *prop=NULL;
    if (pick->PickProp(X, Y, ren, col) ) {
      prop = pick->GetViewProp();
    }
    if (prop) {
      for (int ix1=0; ix1<9; ix1++) {
        if (prop == m_pointActor[ix1]) {
          m_currPropIndex = ix1;
        }
      }
      if (m_currPropIndex > -1) {
        m_pointActor[m_currPropIndex]->GetProperty()->SetColor(1.0, 0.0, 0.0);
      }
    }
  }
}

void ObjectControlWidget::mouseMoveEvent(QMouseEvent* event) {
  if(!m_showing || !event || m_currPropIndex == -1) return;

  QSize winSize = rtObjectManager::instance().getMainWinHandle()->getRenderWidget()->size();
  int X = event->x();
  int Y = winSize.height()-event->y();

  double cameraRight[3];
  double cameraUp[3];
  double temp[3];
  double dist = rtObjectManager::instance().getMainWinHandle()->getCameraDistance();
  double xdiff = (X-m_oldX)*(dist)/1200.0f;
  double ydiff = (Y-m_oldY)*(dist)/1200.0f;

  double rotate = (X-m_oldX)+(Y-m_oldY);

  rtObjectManager::instance().getMainWinHandle()->getCameraRight(cameraRight);
  rtObjectManager::instance().getMainWinHandle()->getCameraUp(cameraUp);

  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }

  switch (m_currPropIndex) {
    case 0:
    case 8:
    for (int ix1=0; ix1<3 ;ix1++) {
      temp[ix1] = m_convertedLocations[2][ix1] - m_convertedLocations[4][ix1];
    }
    m_transform->PostMultiply();
    m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
    m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
    m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
    m_transform->PreMultiply();
    break;
    case 1:
    case 7:
    for (int ix1=0; ix1<3 ;ix1++) {
      temp[ix1] = m_convertedLocations[3][ix1] - m_convertedLocations[4][ix1];
    }
    m_transform->PostMultiply();
    m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
    m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
    m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
    m_transform->PreMultiply();
    break;
    case 2:
    case 6:
    for (int ix1=0; ix1<3 ;ix1++) {
      temp[ix1] = m_convertedLocations[0][ix1] - m_convertedLocations[4][ix1];
    }
    m_transform->PostMultiply();
    m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
    m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
    m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
    m_transform->PreMultiply();
    break;
    case 3:
    case 5:
    for (int ix1=0; ix1<3 ;ix1++) {
      temp[ix1] = m_convertedLocations[1][ix1] - m_convertedLocations[4][ix1];
    }
    m_transform->PostMultiply();
    m_transform->Translate(-m_convertedLocations[4][0], -m_convertedLocations[4][1], -m_convertedLocations[4][2]);
    m_transform->RotateWXYZ(rotate, temp[0], temp[1], temp[2]);
    m_transform->Translate(m_convertedLocations[4][0], m_convertedLocations[4][1], m_convertedLocations[4][2]);
    m_transform->PreMultiply();
    break;
    break;
    case 4:
    temp[0] = cameraRight[0]*xdiff+cameraUp[0]*ydiff;
    temp[1] = cameraRight[1]*xdiff+cameraUp[1]*ydiff;
    temp[2] = cameraRight[2]*xdiff+cameraUp[2]*ydiff;

    m_transform->PostMultiply();
    m_transform->Translate(temp[0], temp[1], temp[2]);
    m_transform->PreMultiply();
    break;
    default:
    break;
  }
  updateWidgetPosition();
  m_oldX = X;
  m_oldY = Y;
}

void ObjectControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing) return;
  if (event->button() == Qt::LeftButton) {
    // Clear the colors
    for (int ix1=0; ix1<9; ix1++) {
      m_pointActor[ix1]->GetProperty()->SetColor(1.0, 1.0, 1.0);
    }
    m_currPropIndex = -1;
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

}

void ObjectControlWidget::updateWidgetPosition() {

  for (int ix1=0; ix1<9; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }
  for (int ix1=0; ix1<9; ix1++) {
    m_touchPoints[ix1]->SetCenter(m_convertedLocations[ix1]);
    m_touchPoints[ix1]->SetRadius(m_size*0.2);
  }

  for (int ix1=0; ix1<4; ix1++) {
    m_edges[ix1]->SetPoint1(m_convertedLocations[m_corners[ix1]][0], m_convertedLocations[m_corners[ix1]][1], m_convertedLocations[m_corners[ix1]][2]);
    m_edges[ix1]->SetPoint2(m_convertedLocations[m_corners[(ix1+1)%4]][0], m_convertedLocations[m_corners[(ix1+1)%4]][1], m_convertedLocations[m_corners[(ix1+1)%4]][2]);
    m_edges[ix1]->SetResolution(1);
  }
}
