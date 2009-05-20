#include "rt2dSliceRenderObject.h"
#include "rt2dSliceDataObject.h"

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
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Add this object to the given renderer.
bool rt2DSliceRenderObject::addToRenderer(vtkRenderer* ren) {

}

//! Remove this object from the given renderer.
bool rt2DSliceRenderObject::removeFromRenderer(vtkRenderer* ren) {

}

//! Create the correct data object.
void rt2DSliceRenderObject::setupDataObject() {
  m_dataObj = new rt2DSliceDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt2DSliceRenderObject::setupPipeline() {
}
