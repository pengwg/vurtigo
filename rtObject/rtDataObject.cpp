#include "rtDataObject.h"

rtDataObject::rtDataObject() {
}

rtDataObject::~rtDataObject() {
}


rtRenderOptions* rtDataObject::getRenderOptions() {
  return m_renderOptions;
}

void rtDataObject::setRenderOptions(rtRenderOptions* renOpt) {
  m_renderOptions = renOpt;
}


int rtDataObject::getId() {
  return m_objId;
}

void rtDataObject::setId(int id) {
  m_objId = id;
}

  
QString rtDataObject::getObjName(){
  return m_objName;
}

void rtDataObject::setObjName(QString name) {
  m_objName = name;
}


rtConstants::rtObjectType rtDataObject::getObjectType() {
  return m_objType;
}

void rtDataObject::setObjectType(rtConstants::rtObjectType ot) {
  m_objType = ot;
}
