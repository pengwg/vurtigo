#include "rtMatrixRenderObject.h"
#include "rtMatrixDataObject.h"

rtMatrixRenderObject::rtMatrixRenderObject() {
  setObjectType(rtConstants::OT_vtkMatrix4x4);
  setName("Matrix Renderer");
  setupDataObject();
  setupPipeline();
}


rtMatrixRenderObject::~rtMatrixRenderObject() {
}


//! Take info from the data object. 
void rtMatrixRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Add this object to the given renderer.
bool rtMatrixRenderObject::addToRenderer(vtkRenderer* ren) {

}

//! Remove this object from the given renderer.
bool rtMatrixRenderObject::removeFromRenderer(vtkRenderer* ren) {

}

//! Create the correct data object.
void rtMatrixRenderObject::setupDataObject() {
  m_dataObj = new rtMatrixDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtMatrixRenderObject::setupPipeline() {
}
