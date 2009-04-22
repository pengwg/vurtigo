#include "rt3dVolumeRenderObject.h"
#include "rt3dVolumeDataObject.h"

rt3DVolumeRenderObject::rt3DVolumeRenderObject() {
  setObjectType(rtConstants::OT_3DObject);
  setName("3D Volume Renderer");
  setupDataObject();
  setupPipeline();
}


rt3DVolumeRenderObject::~rt3DVolumeRenderObject() {
}


//! Take info from the data object. 
void rt3DVolumeRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rt3DVolumeRenderObject::setupDataObject() {
  m_dataObj = new rt3DVolumeDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt3DVolumeRenderObject::setupPipeline() {
}
