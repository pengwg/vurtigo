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
#include "rtPieceFuncDataObject.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

//! Constructor
rtPieceFuncDataObject::rtPieceFuncDataObject() {
  setObjectType("OT_vtkPiecewiseFunction");
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
  rtDataObject::saveHeader(&writer, getObjectType(), getObjName());

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
  QString objType;
  QString objName="";

  while (!reader.atEnd()) {
    if(reader.readNext() == QXmlStreamReader::StartElement) {
      if (reader.name() == "FileInfo") {
        rtDataObject::loadHeader(&reader, objType, objName);
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
