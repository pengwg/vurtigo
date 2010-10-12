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

  setupDataObject();
  setupPipeline();
}


rt3DPointBufferRenderObject::~rt3DPointBufferRenderObject() {
}


//! Take info from the data object. 
void rt3DPointBufferRenderObject::update() {
  rt3DPointBufferDataObject *dObj = dynamic_cast<rt3DPointBufferDataObject*>(m_dataObj);
  rtSingle3DPointPipeline *tempPipe;
  double ptIn[3];
  QList<rtBasic3DPointData>* pointList = dObj->getPointList();
  QList<int>* selectedList = dObj->getSelectedItemsList();

  // Find the total size of the list.
  int totalActors = pointList->size() + selectedList->size();
  int listPosition = 0;
  double rc, gc, bc;
  // Resize the list
  resizePipeList(totalActors);

  // Get the new list
  for (int ix1=0; ix1<pointList->size(); ix1++) {

    tempPipe = m_pipeList[listPosition];
    listPosition++;

    (*pointList)[ix1].getPoint(ptIn);

    tempPipe->setPosition(ptIn[0], ptIn[1], ptIn[2]);
    tempPipe->setProperty( (*pointList)[ix1].getProperty() );
    tempPipe->setRadius( (*pointList)[ix1].getPointSize() );

    // If this point is selected then add the selection sphere.
    if ( selectedList->contains((*pointList)[ix1].getPointId()) ) {
      tempPipe = m_pipeList[listPosition];
      listPosition++;
      tempPipe->setProperty( (*pointList)[ix1].getSelectedProperty() );
      tempPipe->setPosition(ptIn[0], ptIn[1], ptIn[2]);
      // Larger Radius
      tempPipe->setRadius( (*pointList)[ix1].getPointSize()*2.0 );
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
  return true;
}

bool rt3DPointBufferRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
  for (int ix1=0; ix1<m_pipeList.count(); ix1++) {
    if (ren->HasViewProp( m_pipeList.at(ix1)->getActor() )) ren->RemoveViewProp( m_pipeList.at(ix1)->getActor() );
  }
  return true;
}

void rt3DPointBufferRenderObject::setRenderQuality(double quality) {
  double q;
  if (quality < 0.0) q = 0.0;
  else if (quality > 1.0) q = 1.0;
  else q = quality;

  for (int ix1=0; ix1<m_pipeList.size(); ix1++) {
    m_pipeList[ix1]->setResolution(q*10.0f+5);
  }
}


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

  QList<rtBasic3DPointData>* ptList = dObj->getPointList();
  if (!ptList) return false;
  if (ptList->count() <= 0) return false;

  // Just use the first point as the default.
  int ptSize = (*ptList)[0].getPointSize();
  loc[0] = (*ptList)[0].getX() - ptSize; loc[1] = (*ptList)[0].getX() + ptSize; // x
  loc[2] = (*ptList)[0].getY() - ptSize; loc[3] = (*ptList)[0].getY() + ptSize; // y
  loc[4] = (*ptList)[0].getZ() - ptSize; loc[5] = (*ptList)[0].getZ() + ptSize; // z

  for (int ix1=1; ix1<ptList->count(); ix1++) {
    // X
    double xVal = (*ptList)[ix1].getX();
    double yVal = (*ptList)[ix1].getY();
    double zVal = (*ptList)[ix1].getZ();

    if ( ( xVal - ptSize ) < loc[0]) {
      loc[0] = xVal - ptSize;
    } else if ( (xVal + ptSize) > loc[1]) {
      loc[1] = xVal + ptSize;
    }

    // Y
    if ( (yVal - ptSize) < loc[2]) {
      loc[2] = yVal - ptSize;
    } else if ( (yVal + ptSize) > loc[3]) {
      loc[3] = yVal + ptSize;
    }

    // Z
    if ( (zVal - ptSize) < loc[4]) {
      loc[4] = zVal - ptSize;
    } else if ( (zVal + ptSize) > loc[5]) {
      loc[5] = zVal + ptSize;
    }
  }

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
