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
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rtPieceFuncRenderObject::setupDataObject() {
  m_dataObj = new rtPieceFuncDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPieceFuncRenderObject::setupPipeline() {
}
