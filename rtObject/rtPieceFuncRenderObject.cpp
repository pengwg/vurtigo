#include "rtPieceFuncRenderObject.h"
#include "rtPieceFuncDataObject.h"

rtPieceFuncRenderObject::rtPieceFuncRenderObject() {
  setObjectType(rtConstants::OT_vtkPiecewiseFunction);
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
bool rtPieceFuncRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;

  return true;
}

//! Remove this object from the given renderer.
bool rtPieceFuncRenderObject::removeFromRenderer(vtkRenderer* ren) {
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
