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
}

//! Add this object to the given renderer.
bool rtImageBufferRenderObject::addToRenderer(vtkRenderer* ren) {

}

//! Remove this object from the given renderer.
bool rtImageBufferRenderObject::removeFromRenderer(vtkRenderer* ren) {

}

//! Create the correct data object.
void rtImageBufferRenderObject::setupDataObject() {
  m_dataObj = new rtImageBufferDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtImageBufferRenderObject::setupPipeline() {
}

//! When this object is implemented it will have a valid location. For now it does not.
bool rtImageBufferRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
