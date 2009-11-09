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
#include "rtPieceFuncDataObject.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

//! Constructor
rtPieceFuncDataObject::rtPieceFuncDataObject() {
  setObjectType(rtConstants::OT_vtkPiecewiseFunction);
  m_pieceFunc = vtkPiecewiseFunction::New();
  setupGUI();
}

//! Destructor
rtPieceFuncDataObject::~rtPieceFuncDataObject() {
  cleanupGUI();
  m_pieceFunc->Delete();
}

//! Copy a new piecewise function into this object.
bool rtPieceFuncDataObject::setPiecewiseFunction(vtkPiecewiseFunction* piece) {
  if (!piece) return false;
  m_pieceFunc->DeepCopy(piece);
  m_graph->setPiecewiseFunction(m_pieceFunc);
  return true;
}


void rtPieceFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtPieceFuncDataObject::setupGUI() {
  m_graph = new VtkPiecewiseGraph();
  QWidget *wid = getBaseWidget();
  m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, wid);

  connect(m_graph, SIGNAL(functionUpdated()), this, SLOT(Modified()));

  wid->setLayout(m_mainLayout);
  m_mainLayout->addWidget(m_graph);
}

//! Clean the GUI widgets.
void rtPieceFuncDataObject::cleanupGUI() {
  m_mainLayout->removeWidget(m_graph);
  delete m_mainLayout;
  delete m_graph;
}

bool rtPieceFuncDataObject::saveFile(QFile* file) {
  if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QXmlStreamWriter writer(file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeStartElement("VurtigoFile");
  writer.writeStartElement("FileInfo");
  writer.writeTextElement( "type", QString::number( (int)getObjectType() ) );
  writer.writeTextElement( "name", getObjName() );
  writer.writeEndElement();

  vtkPiecewiseFunction* piece;
  piece = getPiecewiseFunction();
  double range[2];
  double node[4];
  int numPoints = piece->GetSize();
  piece->GetRange(range);

  writer.writeStartElement("PieceFuncData");
  writer.writeTextElement( "minBounds", QString::number(range[0]) );
  writer.writeTextElement( "maxBounds", QString::number(range[1]) );
  writer.writeTextElement( "clamping", QString::number(piece->GetClamping()) );
  writer.writeEndElement();

  writer.writeStartElement("PieceFuncPoints");
  writer.writeAttribute( "numPoints", QString::number(numPoints) );
  for (int ix1=0; ix1<numPoints; ix1++) {
    piece->GetNodeValue(ix1, node);
    writer.writeStartElement("Node");
    writer.writeAttribute( "x", QString::number(node[0]) );
    writer.writeAttribute( "y", QString::number(node[1]) );
    writer.writeAttribute( "mid", QString::number(node[2]) );
    writer.writeAttribute( "sharp", QString::number(node[3]) );
    writer.writeEndElement();
  }
  writer.writeEndElement();
  writer.writeEndElement(); // VurtigoFile
  writer.writeEndDocument();

  file->close();
  return true;
}

bool rtPieceFuncDataObject::loadFile(QFile* file) {
  if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  QXmlStreamReader reader(file);

  while (!reader.atEnd()) {
    if(reader.readNext() == QXmlStreamReader::StartElement) {
      if (reader.name() == "FileInfo") {
        readFileInfo(&reader);
      } else if (reader.name() == "PieceFuncData") {
        readFuncData(&reader);
      } else if (reader.name() == "PieceFuncPoints") {
        readPointData(&reader);
      }
    }

  }
  if (reader.hasError()) {
    file->close();
    return false;
  }

  m_graph->setPiecewiseFunction(m_pieceFunc);
  file->close();
  return true;
}

//! Internal reading function
void rtPieceFuncDataObject::readFileInfo(QXmlStreamReader* reader) {
  rtConstants::rtObjectType objType;
  QString objName="";
  bool intOK;

  while ( reader->name() != "FileInfo" || !reader->isEndElement() ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {

      if(reader->name() == "type") {
        objType = rtConstants::intToObjectType(reader->readElementText().toInt(&intOK));
      } else if (reader->name() == "name") {
        objName = reader->readElementText();
      }

    }
  }
}

void rtPieceFuncDataObject::readFuncData(QXmlStreamReader* reader) {
  double bounds[2];
  int clamping;
  bool intOK;


  while ( reader->name() != "PieceFuncData" || !reader->isEndElement() ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {

      if (reader->name() == "minBounds") {
        bounds[0] = reader->readElementText().toDouble(&intOK);
      } else if (reader->name() == "maxBounds") {
        bounds[1] = reader->readElementText().toDouble(&intOK);
      } else if (reader->name() == "clamping") {
        clamping = reader->readElementText().toInt(&intOK);
      }
    }
  }

  m_pieceFunc->AdjustRange(bounds);
  m_pieceFunc->SetClamping(clamping);
}

void rtPieceFuncDataObject::readPointData(QXmlStreamReader* reader) {
  QXmlStreamAttributes attrib;
  int numPoints;
  int currPoint;
  double node[4];
  bool intOK;

  m_pieceFunc->RemoveAllPoints();

  attrib = reader->attributes();
  numPoints = attrib.value("numPoints").toString().toInt();
  currPoint = 0;
  while ( currPoint < numPoints ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {
      if (reader->name() == "Node") {
        attrib = reader->attributes();
        node[0] = attrib.value("x").toString().toDouble(&intOK);
        node[1] = attrib.value("y").toString().toDouble(&intOK);
        node[2] = attrib.value("mid").toString().toDouble(&intOK);
        node[3] = attrib.value("sharp").toString().toDouble(&intOK);
        m_pieceFunc->AddPoint(node[0], node[1], node[2], node[3]);
        currPoint++;
      }
    }
  }

}
