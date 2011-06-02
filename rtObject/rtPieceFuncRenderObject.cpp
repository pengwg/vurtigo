#include "rtPieceFuncRenderObject.h"
#include "rtPieceFuncDataObject.h"

rtPieceFuncRenderObject::rtPieceFuncRenderObject() {
  setObjectType("OT_vtkPiecewiseFunction");
  setName("Piecewise Function Renderer");
  setupDataObject();
  setupPipeline();
}


rtPieceFuncRenderObject::~rtPieceFuncRenderObject() {
}


//! Take info from the data object. 
void rtPieceFuncRenderObject::update() {
}

//! Add this object to the given renderer.
bool rtPieceFuncRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;

  return true;
}

//! Remove this object from the given renderer.
bool rtPieceFuncRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;

  return true;
}

//! Create the correct data object.
void rtPieceFuncRenderObject::setupDataObject() {
  m_dataObj = new rtPieceFuncDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPieceFuncRenderObject::setupPipeline() {
}

//! This object has no valid 3D location.
bool rtPieceFuncRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
