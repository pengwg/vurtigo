#include "rt2dPointRenderObject.h"
#include "rt2dPointDataObject.h"

rt2DPointRenderObject::rt2DPointRenderObject() {
  setObjectType(rtConstants::OT_2DPointBuffer);
  setName("2D Point Renderer");
  setupDataObject();
  setupPipeline();
}


rt2DPointRenderObject::~rt2DPointRenderObject() {
}


//! Take info from the data object. 
void rt2DPointRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rt2DPointRenderObject::setupDataObject() {
  m_dataObj = new rt2DPointDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt2DPointRenderObject::setupPipeline() {
}
