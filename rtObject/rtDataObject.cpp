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
#include <vtkProperty.h>

#include "rtDataObject.h"
#include "rtPluginLoader.h"
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtObjectManager.h"


rtDataObject::rtDataObject() {
  int nextID;

  m_readOnly = false;

  // Get a valid ID from the object manager.
  nextID = rtApplication::instance().getObjectManager()->getNextID();
  if (nextID == -1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not find a valid ID for a new object! "));
  }
  setId(nextID);

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

void rtDataObject::Modified() {
  m_modifyTime=QDateTime::currentDateTime();
  rtApplication::instance().getPluginLoader()->objectModified(m_objId);
  emit objectChanged(m_objId);
}

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
  if (!reader) {
    type = rtConstants::OT_None;
    name = "";
    return;
  }

  if ( !(reader->name()=="FileInfo" || reader->isStartElement()) ) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to load file header."));
    return;
  }

  name = "";
  while ( reader->name() != "FileInfo" || !reader->isEndElement() ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {

      if(reader->name() == "type") {
        type = rtConstants::intToObjectType( getIntFromString(reader->readElementText(), rtConstants::OT_None) );
      } else if (reader->name() == "name") {
        name = reader->readElementText();
      }

    }
  }

  rtApplication::instance().getMessageHandle()->log(QString("Finished reading file header."));
}


void rtDataObject::saveVtkProperty(QXmlStreamWriter *writer, vtkProperty* prop, QString name) {
  if (!writer) return;

  double color[3];
  prop->GetColor(color);

  writer->writeStartElement("VtkProperty");
  writer->writeTextElement( "name", name );
  writer->writeTextElement( "interpolation", QString::number( prop->GetInterpolation() ) );
  writer->writeTextElement( "representation", QString::number( prop->GetRepresentation() ) );
  writer->writeTextElement( "colorR", QString::number( color[0] ) );
  writer->writeTextElement( "colorG", QString::number( color[1] ) );
  writer->writeTextElement( "colorB", QString::number( color[2] ) );
  writer->writeTextElement( "ambient", QString::number( prop->GetAmbient() ) );
  writer->writeTextElement( "diffuse", QString::number( prop->GetDiffuse() ) );
  writer->writeTextElement( "specular", QString::number( prop->GetSpecular() ) );
  writer->writeTextElement( "specularPower", QString::number( prop->GetSpecularPower() ) );
  writer->writeTextElement( "opacity", QString::number( prop->GetOpacity() ) );
  writer->writeEndElement();
}


void rtDataObject::loadVtkProperty(QXmlStreamReader *reader, vtkProperty* prop, QString &name) {
  if (!reader) {
    name = "";
    return;
  }

  if ( !(reader->name()=="VtkProperty" || reader->isStartElement()) ) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to load VtkProperty from file."));
    return;
  }

  double color[3] = {1.0,1.0,1.0};
  name = "";
  while ( reader->name() != "VtkProperty" || !reader->isEndElement() ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {  

      if (reader->name() == "name") {
        name = reader->readElementText();
      } else if (reader->name() == "interpolation") {
        prop->SetInterpolation( getIntFromString(reader->readElementText(), VTK_FLAT) );
      } else if (reader->name() == "representation") {
        prop->SetRepresentation( getIntFromString(reader->readElementText(), VTK_SURFACE) );
      } else if (reader->name() == "colorR") {
        color[0] = getDoubleFromString(reader->readElementText(), 1.0);
      } else if (reader->name() == "colorG") {
        color[1] = getDoubleFromString(reader->readElementText(), 1.0);
      } else if (reader->name() == "colorB") {
        color[2] = getDoubleFromString(reader->readElementText(), 1.0);
      } else if (reader->name() == "ambient") {
        prop->SetAmbient( getDoubleFromString(reader->readElementText(), 0.0) );
      } else if (reader->name() == "diffuse") {
        prop->SetDiffuse( getDoubleFromString(reader->readElementText(), 1.0) );
      } else if (reader->name() == "specular") {
        prop->SetSpecular( getDoubleFromString(reader->readElementText(), 0.0) );
      } else if (reader->name() == "specularPower") {
        prop->SetSpecularPower( getDoubleFromString(reader->readElementText(), 1.0) );
      } else if (reader->name() == "opacity") {
        prop->SetOpacity( getDoubleFromString(reader->readElementText(), 1.0) );
      }

    }
  }

  // Ensure the bounds on the color array.
  for (int ix1=0; ix1<3; ix1++) {
    if (color[ix1] < 0.0) color[ix1] = 0.0;
    else if (color[ix1] > 1.0) color[ix1] = 1.0;
  }
  prop->SetColor(color);

}

int rtDataObject::getIntFromString(QString str, int defVal) {
  bool intOK;
  int val = str.toInt(&intOK);

  if (intOK) {
    return val;
  } else {
    return defVal;
  }
}



double rtDataObject::getDoubleFromString(QString str, double defVal) {
  bool doubleOK;
  double val = str.toDouble(&doubleOK);

  if (doubleOK) {
    return val;
  } else {
    return defVal;
  }
}

///////////////////////
// Protected
////////////////////////
