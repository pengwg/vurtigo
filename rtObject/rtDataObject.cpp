#include "rtDataObject.h"
#include "rtPluginLoader.h"

rtDataObject::rtDataObject() {
  m_readOnly = false;
  m_applyButton = new QPushButton("Apply");

  connect(m_applyButton, SIGNAL(pressed()), this, SLOT(apply()));
}

rtDataObject::~rtDataObject() {
  if (m_applyButton) delete m_applyButton;
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

//! The object was just modified. Adjust the time.
void rtDataObject::Modified() {
  m_modifyTime=QDateTime::currentDateTime();
  rtPluginLoader::instance().objectModified(m_objId);
}

//! Get the last time the object was modified.
QDateTime rtDataObject::getModified() {
  return m_modifyTime;
}
