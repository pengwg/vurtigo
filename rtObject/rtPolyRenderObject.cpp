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
}

//! Add this object to the given renderer.
bool rtPolyRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;

  return true;
}

//! Remove this object from the given renderer.
bool rtPolyRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;

  return true;
}

//! Create the correct data object.
void rtPolyRenderObject::setupDataObject() {
  m_dataObj = new rtPolyDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPolyRenderObject::setupPipeline() {
}
