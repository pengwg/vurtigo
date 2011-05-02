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
#include "rtPolyRenderObject.h"
#include "rtPolyDataObject.h"

#include "rtApplication.h"
#include "rtMainWindow.h"
#include "customQVTKWidget.h"
#include "rtMessage.h"

rtPolyRenderObject::rtPolyRenderObject() {
  setObjectType("OT_vtkPolyData");
  setName("Poly Renderer");
  setupDataObject();
  setupPipeline();
}


rtPolyRenderObject::~rtPolyRenderObject() {
  m_actor->Delete();
  m_actor = NULL;
  m_mapper->Delete();
  m_mapper = NULL;
}

//! Take info from the data object. 
void rtPolyRenderObject::update() {
  rtPolyDataObject *dObj = static_cast<rtPolyDataObject*>(m_dataObj);

  if (!dObj) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Update fialed! Data object is NULL."));
    return;
  }

  dObj->getPolyData()->Update();

  m_mapper->SetLookupTable(dObj->getColorTable());
  m_actor->SetProperty(dObj->getProperty());
  m_mapper->SetInput(dObj->getPolyData());

}

//! Add this object to the given renderer.
bool rtPolyRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("addToRenderer failed! Renderer object is NULL."));
    return false;
  }

  setVisible3D(window,true);
  if (!ren->HasViewProp(m_actor)) {
    ren->AddViewProp(m_actor);
  }

  // Connect signals and slots for interaction.
  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);
  // Connect mouse actions
  connect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));


  return true;
}

//! Remove this object from the given renderer.
bool rtPolyRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,false);
  if (ren->HasViewProp(m_actor)) {
    ren->RemoveViewProp(m_actor);
  }

  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));


  return true;
}

//! Create the correct data object.
void rtPolyRenderObject::setupDataObject() {
  m_dataObj = new rtPolyDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPolyRenderObject::setupPipeline() {
  m_actor = vtkActor::New();
  m_mapper = vtkPolyDataMapper::New();

  m_actor->SetMapper(m_mapper);

  m_mapper->SetScalarModeToUsePointData();
  m_mapper->UseLookupTableScalarRangeOn();\
  m_mapper->SetColorModeToMapScalars();

  m_canRender3D = true;
}


bool rtPolyRenderObject::getObjectLocation(double loc[6]) {
  if (!m_actor) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get object location. Object actor is NULL."));
    return false;
  }

  m_actor->GetBounds(loc);
  return true;
}

void rtPolyRenderObject::mousePressEvent(QMouseEvent* event, int window)
{
    rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMousePress(event,window);
}

void rtPolyRenderObject::mouseMoveEvent(QMouseEvent *event,int window)
{
    rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseMove(event,window);
}

void rtPolyRenderObject::mouseReleaseEvent(QMouseEvent *event, int window)
{
    rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseRelease(event,window);
}

void rtPolyRenderObject::mouseDoubleClickEvent(QMouseEvent *event, int window)
{

}

void rtPolyRenderObject::keyPressEvent(QKeyEvent *event, int window)
{

}

void rtPolyRenderObject::keyReleaseEvent(QKeyEvent *event, int window)
{

}

void rtPolyRenderObject::wheelEvent(QWheelEvent *event, int window)
{
    rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseWheel(event,window);
}
