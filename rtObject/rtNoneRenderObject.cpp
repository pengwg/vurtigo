#include "rtNoneRenderObject.h"
#include "rtNoneDataObject.h"

rtNoneRenderObject::rtNoneRenderObject() {
  setObjectType("OT_None");
  setName("None Renderer");
  setupDataObject();
  setupPipeline();
}


rtNoneRenderObject::~rtNoneRenderObject() {
}


//! Take info from the data object. 
void rtNoneRenderObject::update() {
}

//! Add this object to the given renderer.
bool rtNoneRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  return false;
}

//! Remove this object from the given renderer.
bool rtNoneRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  return false;
}

//! Create the correct data object.
void rtNoneRenderObject::setupDataObject() {
  m_dataObj = new rtNoneDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtNoneRenderObject::setupPipeline() {
}

//! This object has no valid 3D location.
bool rtNoneRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
