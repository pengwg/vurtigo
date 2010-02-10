/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "rtDataObject.h"
#include "rtPluginLoader.h"
#include "rtApplication.h"

rtDataObject::rtDataObject() {
  m_readOnly = false;

  // When the object changes we can update the GUI.
  connect(this, SIGNAL(objectChanged(int)), this, SLOT(updateGUI()));
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
  rtApplication::instance().getPluginLoader()->objectModified(m_objId);
  emit objectChanged(m_objId);
}

//! Get the last time the object was modified.
QDateTime rtDataObject::getModified() {
  return m_modifyTime;
}

void rtDataObject::updateGUI() {
  // Call the update function for each object.
  update();
}

void rtDataObject::saveHeader(QXmlStreamWriter *writer) {
  rtDataObject::saveHeader(writer, m_objType, m_objName);
}

void rtDataObject::saveHeader(QXmlStreamWriter *writer, rtConstants::rtObjectType type, QString name) {
  writer->writeStartElement("FileInfo");
  writer->writeTextElement( "type", QString::number( (int)type ) );
  writer->writeTextElement( "name", name );
  writer->writeEndElement();
}

void rtDataObject::loadHeader(QXmlStreamReader *reader, rtConstants::rtObjectType &type, QString &name) {
  bool intOK;

  if ( !(reader->name()=="FileInfo" || reader->isStartElement()) ) return;

  while ( reader->name() != "FileInfo" || !reader->isEndElement() ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {

      if(reader->name() == "type") {
        type = rtConstants::intToObjectType(reader->readElementText().toInt(&intOK));
      } else if (reader->name() == "name") {
        name = reader->readElementText();
      }

    }
  }

}
