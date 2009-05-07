#include "rtImageBufferRenderObject.h"
#include "rtImageBufferDataObject.h"

rtImageBufferRenderObject::rtImageBufferRenderObject() {
  setObjectType(rtConstants::OT_ImageBuffer);
  setName("Image Buffer Renderer");
  setupDataObject();
  setupPipeline();
}


rtImageBufferRenderObject::~rtImageBufferRenderObject() {
}


//! Take info from the data object. 
void rtImageBufferRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rtImageBufferRenderObject::setupDataObject() {
  m_dataObj = new rtImageBufferDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtImageBufferRenderObject::setupPipeline() {
}
