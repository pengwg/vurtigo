/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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

  m_objType = "";

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


QString rtDataObject::getObjectType()
{
    return m_objType;
}

void rtDataObject::setObjectType(QString type)
{
    m_objType = type;
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
  // the default is the QString version
    rtDataObject::saveHeader(writer, m_objType, m_objName);
}

void rtDataObject::saveHeader(QXmlStreamWriter *writer, QString type, QString name)
{
    writer->writeStartElement("FileInfo");
    writer->writeTextElement( "type", type );
    writer->writeTextElement( "name", name );
    writer->writeEndElement();
}


void rtDataObject::loadHeader(QXmlStreamReader *reader, QString &type, QString &name)
{
    if (!reader) {
      type = "";
      name = "";
      return;
    }

    if ( !(reader->name()=="FileInfo" || reader->isStartElement()) ) {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to load file header."));
      return;
    }

    name = "";
    bool ok;
    int tmpi;
    QString tmpType;
    while ( reader->name() != "FileInfo" || !reader->isEndElement() ) {
      if(reader->readNext() == QXmlStreamReader::StartElement) {

        if(reader->name() == "type") {
            tmpType = reader->readElementText();
            tmpi = tmpType.toInt(&ok);
            if (ok)
                type = rtConstants::objectTypeToQString(rtConstants::intToObjectType(tmpi));
            else
                type = tmpType;
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
