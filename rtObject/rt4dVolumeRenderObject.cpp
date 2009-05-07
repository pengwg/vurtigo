#include "rt4dVolumeRenderObject.h"
#include "rt4dVolumeDataObject.h"

rt4DVolumeRenderObject::rt4DVolumeRenderObject() {
  setObjectType(rtConstants::OT_4DObject);
  setName(" Renderer");
  setupDataObject();
  setupPipeline();
}


rt4DVolumeRenderObject::~rt4DVolumeRenderObject() {
}


//! Take info from the data object. 
void rt4DVolumeRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rt4DVolumeRenderObject::setupDataObject() {
  m_dataObj = new rt4DVolumeDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt4DVolumeRenderObject::setupPipeline() {
}
