#include "rtDataObject.h"
#include "rtPluginLoader.h"

rtDataObject::rtDataObject() {
  m_readOnly = false;

  // When the object changes we can update the GUI.
  connect(this, SIGNAL(objectChanged()), this, SLOT(updateGUI()));
}

rtDataObject::~rtDataObject() {
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
  emit objectChanged();
}

//! Get the last time the object was modified.
QDateTime rtDataObject::getModified() {
  return m_modifyTime;
}

void rtDataObject::updateGUI() {
  // Call the update function for each object.
  update();
}
