#include "rtNoneRenderObject.h"
#include "rtNoneDataObject.h"

rtNoneRenderObject::rtNoneRenderObject() {
  setObjectType(rtConstants::OT_None);
  setName("None Renderer");
  setupDataObject();
  setupPipeline();
}


rtNoneRenderObject::~rtNoneRenderObject() {
}


//! Take info from the data object. 
void rtNoneRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Add this object to the given renderer.
bool rtNoneRenderObject::addToRenderer(vtkRenderer* ren) {
  return false;
}

//! Remove this object from the given renderer.
bool rtNoneRenderObject::removeFromRenderer(vtkRenderer* ren) {
  return false;
}

//! Create the correct data object.
void rtNoneRenderObject::setupDataObject() {
  m_dataObj = new rtNoneDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtNoneRenderObject::setupPipeline() {
}
