#include "rtPolyRenderObject.h"
#include "rtPolyDataObject.h"

rtPolyRenderObject::rtPolyRenderObject() {
  setObjectType(rtConstants::OT_vtkPolyData);
  setName("Poly Renderer");
  setupDataObject();
  setupPipeline();
}


rtPolyRenderObject::~rtPolyRenderObject() {
}


//! Take info from the data object. 
void rtPolyRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rtPolyRenderObject::setupDataObject() {
  m_dataObj = new rtPolyDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPolyRenderObject::setupPipeline() {
}
