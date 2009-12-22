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
#include "rt2dSliceRenderObject.h"
#include "rt2dSliceDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"

#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkCellArray.h"

rt2DSliceRenderObject::rt2DSliceRenderObject() {
  setObjectType(rtConstants::OT_2DObject);
  setName("2DSlice Renderer");
  setupDataObject();
  setupPipeline();
}


rt2DSliceRenderObject::~rt2DSliceRenderObject() {

}


//! Take info from the data object.
void rt2DSliceRenderObject::update() {
  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if ( !dObj->isDataValid() || !dObj->getUCharData()) return;

  double scaleRange[2];
  double bounds[6];

  dObj->getUCharData()->Update();
  dObj->getUCharData()->GetScalarRange(scaleRange);
  dObj->getUCharData()->GetBounds(bounds);

  m_texturePlane.setImageData(dObj->getUCharData());
  m_texturePlane.setScalarRange(scaleRange[0], scaleRange[1]);
  m_texturePlane.setWindow(dObj->getWindow());
  m_texturePlane.setLevel(dObj->getLevel());

  m_texturePlane.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2]);
  m_texturePlane.setTransform(dObj->getTransform());
  m_texturePlane.update();

  m_boxOutline.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2]);
  m_boxOutline.setTransform(dObj->getTransform());
  m_boxOutline.update();

  m_control.setTransform(dObj->getTransform());
  m_control.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2] );
}

//! Add this object to the given renderer.
bool rt2DSliceRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if (!dObj->isDataValid()) return false;

  // Update the object.
  dObj->Modified();
  this->tryUpdate();

  if(!ren->HasViewProp(m_texturePlane.getActor())) {
    ren->AddViewProp(m_texturePlane.getActor());
  }

  if(!ren->HasViewProp(m_boxOutline.getActor())) {
    ren->AddViewProp(m_boxOutline.getActor());
  }

  customQVTKWidget* renWid;
  renWid = rtObjectManager::instance().getMainWinHandle()->getRenderWidget();
  // Connect mouse actions
  connect(renWid, SIGNAL(interMousePress(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interKeyPress(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
  connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
  connect(renWid, SIGNAL(interWheel(QWheelEvent*)), this, SLOT(wheelEvent(QWheelEvent*)));
  return true;
}

//! Remove this object from the given renderer.
bool rt2DSliceRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);

  if(ren->HasViewProp(m_texturePlane.getActor())) {
    ren->RemoveViewProp(m_texturePlane.getActor());
  }

  if(ren->HasViewProp(m_boxOutline.getActor())) {
    ren->RemoveViewProp(m_boxOutline.getActor());
  }

  customQVTKWidget* renWid;
  renWid = rtObjectManager::instance().getMainWinHandle()->getRenderWidget();

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*)), this, SLOT(wheelEvent(QWheelEvent*)));

  return true;
}

void rt2DSliceRenderObject::mousePressEvent(QMouseEvent* event) {
  if (!m_selectedProp) return;
  if (m_control.isShowing()) {
    m_control.mousePressEvent(event);
  }
}

void rt2DSliceRenderObject::mouseMoveEvent(QMouseEvent* event) {
  if (!m_selectedProp) return;
  if (m_control.isShowing()) {
    m_control.mouseMoveEvent(event);
  }
}

void rt2DSliceRenderObject::mouseReleaseEvent(QMouseEvent* event) {
  if (!m_selectedProp) return;
  if (m_control.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
    m_control.mouseReleaseEvent(event);
    m_control.getTransform(t);
    dObj->setVtkMatrix(t->GetMatrix(), true);
    dObj->Modified();
    t->Delete();
  }
}

void rt2DSliceRenderObject::mouseDoubleClickEvent(QMouseEvent* event) {
  vtkProp* temp;

  temp = rtObjectManager::instance().getMainWinHandle()->getSelectedProp();
  m_selectedProp = NULL;

  if (m_control.isShowing())
    m_control.hide();

  if (temp) {
    if (temp == m_texturePlane.getActor() || temp == m_boxOutline.getActor()) {
      m_selectedProp = temp;
      m_control.show();
    }
  }

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);

  if (m_selectedProp) {
    dObj->setManualOn();
  } else {
    dObj->setManualOff();
  }
}

void rt2DSliceRenderObject::keyPressEvent(QKeyEvent* event) {
  if (!m_selectedProp) return;
}

void rt2DSliceRenderObject::keyReleaseEvent(QKeyEvent* event) {
  if (!m_selectedProp) return;
}

void rt2DSliceRenderObject::wheelEvent(QWheelEvent* event) {
  if (!m_selectedProp) return;
  if (m_control.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
    m_control.wheelEvent(event);
    m_control.getTransform(t);
    dObj->setVtkMatrix(t->GetMatrix(), true);
    dObj->Modified();
    t->Delete();
  }
}

//! Create the correct data object.
void rt2DSliceRenderObject::setupDataObject() {
  m_dataObj = new rt2DSliceDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt2DSliceRenderObject::setupPipeline() {
  // Objects for the texture pipeline.
  m_pipe3D.push_back( m_texturePlane.getActor() );
  m_pipe3D.push_back( m_boxOutline.getActor() );
}

//! The position of the center of the plane is given
bool rt2DSliceRenderObject::getObjectLocation(double loc[6]) {
  if (!m_boxOutline.getActor()) return false;

  m_boxOutline.getActor()->GetBounds(loc);

  return true;
}
