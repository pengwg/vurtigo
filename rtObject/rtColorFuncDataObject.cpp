#include "rtColorFuncDataObject.h"

//! Constructor
rtColorFuncDataObject::rtColorFuncDataObject() {
  setObjectType("OT_vtkColorTransferFunction");
  m_ctf = vtkColorTransferFunction::New();
  setupGUI();
}

//! Destructor
rtColorFuncDataObject::~rtColorFuncDataObject() {
  cleanupGUI();
  m_ctf->Delete();
}

//! Set a new color funciton.
bool rtColorFuncDataObject::setColorFunction(vtkColorTransferFunction* ctf) {
  if (!ctf) return false;
  m_ctf->DeepCopy(ctf);
  m_table->setColorFunction(m_ctf);
  return true;
}

//! Send the info to the GUI
void rtColorFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtColorFuncDataObject::setupGUI() {
  m_table = new VtkColorTable();
  QWidget *wid = getBaseWidget();
  m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, wid);

  connect(m_table, SIGNAL(functionUpdated()), this, SLOT(Modified()));

  wid->setLayout(m_mainLayout);
  m_mainLayout->addWidget(m_table);
}

//! Clean the GUI widgets.
void rtColorFuncDataObject::cleanupGUI() {
  m_mainLayout->removeWidget(m_table);
  delete m_mainLayout;
  delete m_table;
}

bool rtColorFuncDataObject::saveFile(QFile *file) {
  if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QXmlStreamWriter writer(file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeStartElement("VurtigoFile");
  rtDataObject::saveHeader(&writer, getObjectType(), getObjName());

  vtkColorTransferFunction* color;
  color = getColorFunction();
  double range[2];
  double node[6];
  int numPoints = color->GetSize();
  color->GetRange(range);

  writer.writeStartElement("ColorFuncData");
  writer.writeTextElement( "minBounds", QString::number(range[0]) );
  writer.writeTextElement( "maxBounds", QString::number(range[1]) );
  writer.writeTextElement( "clamping", QString::number(color->GetClamping()) );
  writer.writeEndElement();

  writer.writeStartElement("ColorFuncPoints");
  writer.writeAttribute( "numPoints", QString::number(numPoints) );
  for (int ix1=0; ix1<numPoints; ix1++) {
    color->GetNodeValue(ix1, node);
    writer.writeStartElement("Node");
    writer.writeAttribute( "x", QString::number(node[0]) );
    writer.writeAttribute( "r", QString::number(node[1]) );
    writer.writeAttribute( "g", QString::number(node[2]) );
    writer.writeAttribute( "b", QString::number(node[3]) );
    writer.writeAttribute( "mid", QString::number(node[4]) );
    writer.writeAttribute( "sharp", QString::number(node[5]) );
    writer.writeEndElement();
  }
  writer.writeEndElement();

  writer.writeEndElement(); // VurtigoFile
  writer.writeEndDocument();


  file->close();
  return true;
}

bool rtColorFuncDataObject::loadFile(QFile *file) {
  if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  QXmlStreamReader reader(file);
  QString objType;
  QString objName="";

  while (!reader.atEnd()) {
    if(reader.readNext() == QXmlStreamReader::StartElement) {
      if (reader.name() == "FileInfo") {
        rtDataObject::loadHeader(&reader, objType, objName);
      } else if (reader.name() == "ColorFuncData") {
        readFuncData(&reader);
      } else if (reader.name() == "ColorFuncPoints") {
        readPointData(&reader);
      }
    }

  }
  if (reader.hasError()) {
    file->close();
    return false;
  }

  m_table->setColorFunction(m_ctf);
  file->close();
  return true;
}

void rtColorFuncDataObject::readFuncData(QXmlStreamReader* reader) {
  double bounds[2];
  int clamping;
  bool intOK;

  while ( reader->name() != "ColorFuncData" || !reader->isEndElement() ) {
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

  m_ctf->AdjustRange(bounds);
  m_ctf->SetClamping(clamping);
}

void rtColorFuncDataObject::readPointData(QXmlStreamReader* reader) {
  QXmlStreamAttributes attrib;
  int numPoints;
  int currPoint;
  double node[6];
  bool intOK;

  m_ctf->RemoveAllPoints();

  attrib = reader->attributes();
  numPoints = attrib.value("numPoints").toString().toInt();
  currPoint = 0;
  while ( currPoint < numPoints ) {
    if(reader->readNext() == QXmlStreamReader::StartElement) {
      if (reader->name() == "Node") {
        attrib = reader->attributes();
        node[0] = attrib.value("x").toString().toDouble(&intOK);
        node[1] = attrib.value("r").toString().toDouble(&intOK);
        node[2] = attrib.value("g").toString().toDouble(&intOK);
        node[3] = attrib.value("b").toString().toDouble(&intOK);
        node[4] = attrib.value("mid").toString().toDouble(&intOK);
        node[5] = attrib.value("sharp").toString().toDouble(&intOK);
        m_ctf->AddRGBPoint(node[0], node[1], node[2], node[3], node[4], node[5]);
        currPoint++;
      }
    }
  }
}
