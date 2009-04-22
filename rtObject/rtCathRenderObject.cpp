#include "rtCathRenderObject.h"
#include "rtCathDataObject.h"

rtCathRenderObject::rtCathRenderObject() {
  setObjectType(rtConstants::OT_Cath);
  setName("Cath Renderer");
  setupDataObject();
  setupPipeline();
}


rtCathRenderObject::~rtCathRenderObject() {
}


//! Take info from the data object. 
void rtCathRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rtCathRenderObject::setupDataObject() {
  m_dataObj = new rtCathDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtCathRenderObject::setupPipeline() {
}
