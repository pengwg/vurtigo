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
  // And the z directions
  m_pointLocations[9][0] = 1;
  m_pointLocations[9][1] = 1;
  m_pointLocations[9][2] = 1; // Positive z

  m_pointLocations[10][0] = 1;
  m_pointLocations[10][1] = 1;
  m_pointLocations[10][2] = -1; // Negative z

  m_corners[0] = 0;
  m_corners[1] = 2;
  m_corners[2] = 8;
  m_corners[3] = 6;

  // There is no current prop yet
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

void ObjectControlWidget::setSize(double xsize, double ysize, double zsize) {
  m_xsize = xsize;
  m_ysize = ysize;
  m_zsize = zsize;

  // When the size changes so do the point locations.
  for (int ix1=0; ix1<3; ix1++) {
    for (int ix2=0; ix2<3; ix2++) {
      m_pointLocations[ix1+3*ix2][0] = (ix1)*m_xsize*0.5;
      m_pointLocations[ix1+3*ix2][1] = (ix2)*m_ysize*0.5;
      m_pointLocations[ix1+3*ix2][2] = 0.0f;  // The z is centered.
    }
  }

  // And the z directions
  m_pointLocations[9][0] = 1*m_xsize*0.5;
  m_pointLocations[9][1] = 1*m_ysize*0.5;
  m_pointLocations[9][2] = 1*m_zsize*0.5; // Positive z

  m_pointLocations[10][0] = 1*m_xsize*0.5;
  m_pointLocations[10][1] = 1*m_ysize*0.5;
  m_pointLocations[10][2] = -1*m_zsize*0.5; // Negative z

  sizeChanged();
}

void ObjectControlWidget::sizeChanged() {
  // Empty in this class
}

void ObjectControlWidget::show() {
  if(m_showing) return;
  m_showing = true;
  visibilityChanged();
}

void ObjectControlWidget::hide() {
  if(!m_showing) return;
  m_showing = false;
  visibilityChanged();
}

bool ObjectControlWidget::isShowing() {
  return m_showing;
}

void ObjectControlWidget::visibilityChanged() {
  // Empty Here
}


void ObjectControlWidget::setUserTransform(vtkTransform* t) {
  if (!t) return;
  m_userTransform->Identity();
  m_userTransform->Concatenate(t);
  userTransformChanged();
}

void ObjectControlWidget::userTransformChanged() {
  // Empty here
}

void ObjectControlWidget::setWidgetOpacity(double op) {
  if (op > 1.0) op = 1.0;
  else if (op < 0.0) op = 0.0;
  m_widgetOpacity = op;
  widgetOpacityChanged();
}

double ObjectControlWidget::getWidgetOpacity() {
  return m_widgetOpacity;
}

void ObjectControlWidget::widgetOpacityChanged() {
  // Empty in the base class
}

/////////////////
// Public Slots
/////////////////
void ObjectControlWidget::mousePressEvent(QMouseEvent* event) {
  if(!m_showing || !event) return;
}

void ObjectControlWidget::mouseMoveEvent(QMouseEvent* event) {
  if(!m_showing || !event) return;
}

void ObjectControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing || !event) return;
}

void ObjectControlWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  if(!m_showing || !event) return;

}

void ObjectControlWidget::keyPressEvent(QKeyEvent* event) {
  if(!m_showing || !event) return;

}

void ObjectControlWidget::keyReleaseEvent(QKeyEvent* event) {
  if(!m_showing || !event) return;

}

void ObjectControlWidget::wheelEvent(QWheelEvent* event) {
  if(!m_showing || !event) return;
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
  // Empty in the base class.
}

void ObjectControlWidget::updateConvertedLocations() {
  for (int ix1=0; ix1<11; ix1++) {
    m_transform->TransformPoint(m_pointLocations[ix1], m_convertedLocations[ix1]);
  }
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
