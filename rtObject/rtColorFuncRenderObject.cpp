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
}

//! Add this object to the given renderer.
bool rtColorFuncRenderObject::addToRenderer(vtkRenderer* ren) {

}

//! Remove this object from the given renderer.
bool rtColorFuncRenderObject::removeFromRenderer(vtkRenderer* ren) {

}

//! Create the correct data object.
void rtColorFuncRenderObject::setupDataObject() {
  m_dataObj = new rtColorFuncDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtColorFuncRenderObject::setupPipeline() {
}
