#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"

rt3DPointBufferRenderObject::rt3DPointBufferRenderObject() {
  m_pipeList.clear();

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
  SinglePointPipeline *tempPipe;
  bool visible = false;

  if (getVisible3D()) visible = true;

  // Clean the previous list.
  while(!m_pipeList.empty()) {
    tempPipe = m_pipeList.takeFirst();
    m_pipe3D.removeAll(tempPipe->actor);
    if (visible) rtObjectManager::instance().getMainWinHandle()->removeRenderItem(tempPipe->actor);
    delete tempPipe;
  }

  // Get the new list
  QList<rt3DPointBufferDataObject::SimplePoint>* pointList = dObj->getPointList();
  for (int ix1=0; ix1<pointList->size(); ix1++) {
    tempPipe = new SinglePointPipeline();
    tempPipe->sphere->SetThetaResolution(50);
    tempPipe->sphere->SetPhiResolution(50);
    tempPipe->sphere->SetRadius(pointList->at(ix1).pSize);
    tempPipe->sphere->SetCenter(pointList->at(ix1).px, pointList->at(ix1).py, pointList->at(ix1).pz);
    tempPipe->actor->SetProperty(pointList->at(ix1).pProp);

    m_pipeList.append(tempPipe);
    m_pipe3D.push_back(tempPipe->actor);
    if (visible) {
      rtObjectManager::instance().getMainWinHandle()->addRenderItem(tempPipe->actor);
    }
  }
  
  // May need to update the tree item.
  updateTreeItem();
}

//! Add this object to the given renderer.
bool rt3DPointBufferRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);
  for (int ix1=0; ix1<m_pipe3D.count(); ix1++) {
    if (!ren->HasViewProp(m_pipe3D.at(ix1))) {
      ren->AddViewProp(m_pipe3D.at(ix1));
    }
  }
  return true;
}

//! Remove this object from the given renderer.
bool rt3DPointBufferRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
  for (int ix1=0; ix1<m_pipe3D.count(); ix1++) {
    if (ren->HasViewProp(m_pipe3D.at(ix1))) ren->RemoveViewProp(m_pipe3D.at(ix1));
  }
  return true;
}

//! Create the correct data object.
void rt3DPointBufferRenderObject::setupDataObject() {
  m_dataObj = new rt3DPointBufferDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt3DPointBufferRenderObject::setupPipeline() {
  //Nothing to do here since the list is empty to start with. 
}
