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
#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

//VTK
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

rt3DPointBufferRenderObject::rt3DPointBufferRenderObject() {
  setObjectType(rtConstants::OT_3DPointBuffer);
  setName("Simple 3D Point Renderer");

  // Clean the pipe list before the pipeline is set up.
  cleanupPipeList();
  setupDataObject();
  setupPipeline();
}


rt3DPointBufferRenderObject::~rt3DPointBufferRenderObject() {
}


//! Take info from the data object. 
void rt3DPointBufferRenderObject::update() {
  rt3DPointBufferDataObject *dObj = dynamic_cast<rt3DPointBufferDataObject*>(m_dataObj);
  rtSingle3DPointPipeline *tempPipe;

  // Cleaup the old list.
  cleanupPipeList();

  // Get the new list
  QList<rt3DPointBufferDataObject::SimplePoint>* pointList = dObj->getPointList();
  for (int ix1=0; ix1<pointList->size(); ix1++) {
    tempPipe = new rtSingle3DPointPipeline();
    tempPipe->setResolution(20);
    tempPipe->setRadius(pointList->at(ix1).pSize);
    tempPipe->setPosition(pointList->at(ix1).px, pointList->at(ix1).py, pointList->at(ix1).pz);
    tempPipe->setProperty(pointList->at(ix1).pProp);

    m_pipeList.append(tempPipe);
    if (getVisible3D()) {
      rtApplication::instance().getMainWinHandle()->addRenderItem(tempPipe->getActor());
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
    m_pipeList[ix1]->setResolution(q*40.0f+5);
  }
}


void rt3DPointBufferRenderObject::setupDataObject() {
  m_dataObj = new rt3DPointBufferDataObject();
}


void rt3DPointBufferRenderObject::setupPipeline() {
  //Create a dummy object in the pipeline
  rtSingle3DPointPipeline *tempPipe;
  tempPipe = new rtSingle3DPointPipeline();
  tempPipe->setResolution(10);
  tempPipe->setRadius(1);
  tempPipe->setPosition(0.0f ,0.0f ,0.0f);

  m_pipeList.append(tempPipe);
  m_canRender3D = true;
}


bool rt3DPointBufferRenderObject::getObjectLocation(double loc[6]) {
  rt3DPointBufferDataObject *dObj = dynamic_cast<rt3DPointBufferDataObject*>(m_dataObj);
  if (!dObj) return false;

  QList<rt3DPointBufferDataObject::SimplePoint>* ptList = dObj->getPointList();
  if (!ptList) return false;
  if (ptList->count() <= 0) return false;

  // Just use the first point as the default.
  int ptSize = ptList->at(0).pSize;
  loc[0] = ptList->at(0).px - ptSize; loc[1] = ptList->at(0).px + ptSize; // x
  loc[2] = ptList->at(0).py - ptSize; loc[3] = ptList->at(0).py + ptSize; // y
  loc[4] = ptList->at(0).pz - ptSize; loc[5] = ptList->at(0).pz + ptSize; // z

  for (int ix1=1; ix1<ptList->count(); ix1++) {
    // X
    if ( (ptList->at(ix1).px - ptSize) < loc[0]) {
      loc[0] = ptList->at(ix1).px - ptSize;
    } else if ( (ptList->at(ix1).px + ptSize) > loc[1]) {
      loc[1] = ptList->at(ix1).px + ptSize;
    }

    // Y
    if ( (ptList->at(ix1).py - ptSize) < loc[2]) {
      loc[2] = ptList->at(ix1).py - ptSize;
    } else if ( (ptList->at(ix1).py + ptSize) > loc[3]) {
      loc[3] = ptList->at(ix1).py + ptSize;
    }

    // Z
    if ( (ptList->at(ix1).pz - ptSize) < loc[4]) {
      loc[4] = ptList->at(ix1).pz - ptSize;
    } else if ( (ptList->at(ix1).pz + ptSize) > loc[5]) {
      loc[5] = ptList->at(ix1).pz + ptSize;
    }
  }

  return true;
}


void rt3DPointBufferRenderObject::cleanupPipeList() {
  rtSingle3DPointPipeline *tempPipe;

  while(!m_pipeList.empty()) {
    tempPipe = m_pipeList.takeFirst();
    // Remove the item if it is currently being rendered.
    if (getVisible3D()) rtApplication::instance().getMainWinHandle()->removeRenderItem(tempPipe->getActor());
    delete tempPipe;
  }

}
