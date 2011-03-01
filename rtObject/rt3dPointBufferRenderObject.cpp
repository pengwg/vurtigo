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
// Qt
#include <QTime>

//VTK
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

// Local
#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"
#include "rtApplication.h"
#include "rtBasic3DPointData.h"



rt3DPointBufferRenderObject::rt3DPointBufferRenderObject() {
  setObjectType(rtConstants::OT_3DPointBuffer);
  setName("Simple 3D Point Renderer");

  m_currTransform = vtkTransform::New();
  m_currTransform->Identity();

  // initialize!
  m_selectedProp = NULL;

  setupDataObject();
  setupPipeline();
}


rt3DPointBufferRenderObject::~rt3DPointBufferRenderObject() {
  if (m_currTransform) {
    m_currTransform->Delete();
    m_currTransform = NULL;
  }
}


//! Take info from the data object. 
void rt3DPointBufferRenderObject::update() {
  rt3DPointBufferDataObject *dObj = dynamic_cast<rt3DPointBufferDataObject*>(m_dataObj);
  rtSingle3DPointPipeline *tempPipe;
  double ptIn[3];
  QHash<int, rtNamedInfoPointData>* pointList = dObj->getNamedInfoHash();
  QList<int> hashKeys = pointList->keys();
  QList<int>* selectedList = dObj->getSelectedItemsList();

  // Find the total size of the list.
  int totalActors = hashKeys.size() + selectedList->size();
  int listPosition = 0;
  // Resize the list
  resizePipeList(totalActors);

  // Get the new list
  for (int ix1=0; ix1<hashKeys.size(); ix1++) {

    tempPipe = m_pipeList[listPosition];
    listPosition++;

    (*pointList)[hashKeys[ix1]].getPoint(ptIn);

    tempPipe->setPosition(ptIn[0], ptIn[1], ptIn[2]);
    tempPipe->setProperty( (*pointList)[hashKeys[ix1]].getProperty() );
    tempPipe->setRadius( (*pointList)[hashKeys[ix1]].getPointSize()*dObj->getPointZoom() );

    // If this point is selected then add the selection sphere.
    if ( selectedList->contains((*pointList)[hashKeys[ix1]].getPointId()) ) {
      tempPipe = m_pipeList[listPosition];
      listPosition++;
      tempPipe->setProperty( (*pointList)[hashKeys[ix1]].getSelectedProperty() );
      tempPipe->setPosition(ptIn[0], ptIn[1], ptIn[2]);
      // Larger Radius
      tempPipe->setRadius( (*pointList)[hashKeys[ix1]].getPointSize()*dObj->getPointZoom()*2.0 );
    }

  }
}

bool rt3DPointBufferRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);
  for (int ix1=0; ix1<m_pipeList.count(); ix1++) {
    if (!ren->HasViewProp(m_pipeList.at(ix1)->getActor())) {
      ren->AddViewProp( m_pipeList.at(ix1)->getActor() );
    }
  }

  // Connect signals and slots for interaction.
  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();
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

bool rt3DPointBufferRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
  for (int ix1=0; ix1<m_pipeList.count(); ix1++) {
    if (ren->HasViewProp( m_pipeList.at(ix1)->getActor() )) ren->RemoveViewProp( m_pipeList.at(ix1)->getActor() );
  }

  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget();

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*)), this, SLOT(wheelEvent(QWheelEvent*)));

  if( m_controlWidget.isShowing() ) m_controlWidget.hide();

  return true;
}

void rt3DPointBufferRenderObject::setRenderQuality(double quality) {
  double q;
  if (quality < 0.0) q = 0.0;
  else if (quality > 1.0) q = 1.0;
  else q = quality;

  for (int ix1=0; ix1<m_pipeList.size(); ix1++) {
    m_pipeList[ix1]->setResolution(q*8.0f+5);
  }
}

////////////////
// Public Slots
////////////////

void rt3DPointBufferRenderObject::mousePressEvent(QMouseEvent* event) {
  if (!m_selectedProp)
    {
      //if nothing is selected
      if (!rtApplication::instance().getMainWinHandle()->getRenderWidget()->getChosenProp())
          rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMousePress(event);
      return;
  }

  if ( m_controlWidget.isShowing() ) {
    m_controlWidget.mousePressEvent(event);
    if ( rtApplication::instance().getMainWinHandle() ) rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void rt3DPointBufferRenderObject::mouseMoveEvent(QMouseEvent* event) {
  if (!m_selectedProp)
    {
      //if nothing is selected
      if (!rtApplication::instance().getMainWinHandle()->getRenderWidget()->getChosenProp())
          rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMouseMove(event);
      return;
  }

  if (m_controlWidget.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    m_controlWidget.mouseMoveEvent(event);
    m_controlWidget.getTransform(t);
    setControlTransform(t);
    t->Delete();
  }
}

void rt3DPointBufferRenderObject::mouseReleaseEvent(QMouseEvent* event) {
  if (!m_selectedProp)
    {
      //if nothing is selected
      if (!rtApplication::instance().getMainWinHandle()->getRenderWidget()->getChosenProp())
          rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMouseRelease(event);
      return;
  }

  if (m_controlWidget.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    m_controlWidget.mouseReleaseEvent(event);
    m_controlWidget.getTransform(t);
    setControlTransform(t);
    t->Delete();
  }
}

void rt3DPointBufferRenderObject::mouseDoubleClickEvent(QMouseEvent* event) {
  vtkProp* temp;
  double midPoint[3];
  double pointExt[6];
  double widgetSize[3];

  temp = rtApplication::instance().getMainWinHandle()->getSelectedProp();
  rt3DPointBufferDataObject* dObj = static_cast<rt3DPointBufferDataObject*>(m_dataObj);
  m_selectedProp = NULL;

  if (m_controlWidget.isShowing())
    m_controlWidget.hide();

  if (temp && dObj) {
    for (int ix1=0; ix1<m_pipeList.size(); ix1++) {
      if ( temp == m_pipeList[ix1]->getActor() ) {
        m_selectedProp = temp;

        dObj->getPointListCenter(midPoint);
        dObj->getPointListExtents(pointExt);

        widgetSize[0] = (pointExt[1]-pointExt[0])*2.0;
        widgetSize[1] = (pointExt[3]-pointExt[2])*2.0;
        widgetSize[2] = (pointExt[5]-pointExt[4])*2.0;


        m_currTransform->Identity();
        m_currTransform->Translate(midPoint[0]- widgetSize[0]/2.0, midPoint[1]-widgetSize[1]/2.0, midPoint[2]);
        m_controlWidget.setTransform(m_currTransform);
        m_controlWidget.setSize( widgetSize[0], widgetSize[1], widgetSize[2] );

        m_controlWidget.show();
        break;
      }
    }
  }

  if ( rtApplication::instance().getMainWinHandle() ) rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}

void rt3DPointBufferRenderObject::keyPressEvent(QKeyEvent* event) {
  if (!m_selectedProp) return;
}

void rt3DPointBufferRenderObject::keyReleaseEvent(QKeyEvent* event) {
  if (!m_selectedProp) return;
}

void rt3DPointBufferRenderObject::wheelEvent(QWheelEvent* event) {
  if (!m_selectedProp)
    {
      //if nothing is selected
      if (!rtApplication::instance().getMainWinHandle()->getRenderWidget()->getChosenProp())
          rtApplication::instance().getMainWinHandle()->getRenderWidget()->camTakeOverMouseWheel(event);
      return;
  }

  if (m_controlWidget.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    m_controlWidget.wheelEvent(event);
    m_controlWidget.getTransform(t);
    setControlTransform(t);
    t->Delete();
    if ( rtApplication::instance().getMainWinHandle() ) rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

//////////////////
// Protected
////////////////

void rt3DPointBufferRenderObject::setupDataObject() {
  m_dataObj = new rt3DPointBufferDataObject();
}


void rt3DPointBufferRenderObject::setupPipeline() {
  // Cleanup the pipe list.
  cleanupPipeList();
  m_canRender3D = true;
}


bool rt3DPointBufferRenderObject::getObjectLocation(double loc[6]) {
  rt3DPointBufferDataObject *dObj = dynamic_cast<rt3DPointBufferDataObject*>(m_dataObj);
  if (!dObj) return false;

  dObj->getPointListExtents(loc);

  return true;
}


void rt3DPointBufferRenderObject::cleanupPipeList() {
  rtSingle3DPointPipeline *tempPipe;

  while(!m_pipeList.empty()) {
    tempPipe = m_pipeList.takeLast();
    // Remove the item if it is currently being rendered.
    if (getVisible3D()) rtApplication::instance().getMainWinHandle()->removeRenderItem(tempPipe->getActor());
    delete tempPipe;
  }

}

void rt3DPointBufferRenderObject::resizePipeList(int size) {
  rtSingle3DPointPipeline *tempPipe;

  // Shrink it if it is too long
  while(m_pipeList.size() > size) {
    tempPipe = m_pipeList.takeLast();
    // Remove the item if it is currently being rendered.
    if (getVisible3D()) rtApplication::instance().getMainWinHandle()->removeRenderItem(tempPipe->getActor());
    delete tempPipe;
  }

  // Add elements if the list is too short.
  while (m_pipeList.size() < size) {
    tempPipe = new rtSingle3DPointPipeline();
    m_pipeList.append(tempPipe);
    if (getVisible3D()) rtApplication::instance().getMainWinHandle()->addRenderItem(tempPipe->getActor());
  }

}


void rt3DPointBufferRenderObject::setControlTransform(vtkTransform* t) {
  if (!t) return;

  rt3DPointBufferDataObject* dObj = static_cast<rt3DPointBufferDataObject*>(m_dataObj);
  if(!dObj) return;

  vtkTransform *tFinal = vtkTransform::New();

  tFinal->Identity();
  tFinal->Concatenate( t );
  tFinal->Concatenate( static_cast<vtkTransform*>(m_currTransform->GetInverse()) );
  dObj->applyTransformToPoints(tFinal);
  m_currTransform->SetMatrix(t->GetMatrix());

  m_dataObj->Modified();
  tFinal->Delete();
}
