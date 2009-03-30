#include "rtRenderObject.h"

rtRenderObject::rtRenderObject() {
  m_pipe3D = NULL;
  m_pipe2D = NULL;
  m_dataObj = NULL;
  m_renderName = "None";
  m_objType = rtConstants::OT_None;
}

rtRenderObject::~rtRenderObject() {

}

vtkProp* rtRenderObject::get3DPipeline() {
  return m_pipe3D;
}

vtkProp* rtRenderObject::get2DPipeline() {
  return m_pipe2D;
}

rtDataObject* rtRenderObject::getDataObject() {
  return m_dataObj;
}

QString rtRenderObject::getName() {
  return m_renderName;
}

rtConstants::rtObjectType rtRenderObject::getObjectType() {
  return m_objType;
}

void rtRenderObject::set3DPipeline(vtkProp* pipe3D){
  m_pipe3D = pipe3D;
}

void rtRenderObject::set2DPipeline(vtkProp* pipe2D) {
  m_pipe2D = pipe2D;
}

void rtRenderObject::setDataObject(rtDataObject* dataObj) {
  m_dataObj = dataObj;
}

void rtRenderObject::setName(QString renName) {
  m_renderName = renName;
}

void rtRenderObject::setObjectType(rtConstants::rtObjectType objType) {
  m_objType = objType;
}
