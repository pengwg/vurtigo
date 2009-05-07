#include "rtColorFuncRenderObject.h"
#include "rtColorFuncDataObject.h"

rtColorFuncRenderObject::rtColorFuncRenderObject() {
  setObjectType(rtConstants::OT_vtkColorTransferFunction);
  setName("Color Transfer Func Renderer");
  setupDataObject();
  setupPipeline();
}


rtColorFuncRenderObject::~rtColorFuncRenderObject() {
}


//! Take info from the data object. 
void rtColorFuncRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();
}

//! Create the correct data object.
void rtColorFuncRenderObject::setupDataObject() {
  m_dataObj = new rtColorFuncDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtColorFuncRenderObject::setupPipeline() {
}
