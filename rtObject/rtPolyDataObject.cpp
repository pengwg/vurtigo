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
#include "rtPolyDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"
#include "rtTimeManager.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"

#include <QHash>
#include <QColor>
#include <QString>

#include <vtkPoints.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

//! Constructor
rtPolyDataObject::rtPolyDataObject() {
  setObjectType("OT_vtkPolyData");

  m_cineFrame = new QTimer(this);
  connect( m_cineFrame,SIGNAL(timeout()), this, SLOT(nextVisibleComponent()) );

  setupGUI();

  m_currentPhase = -1;
  m_trigDelayList.clear();
  m_polyData.clear();
  m_polyProperty.clear();
  m_colorLookup.clear();
}

//! Destructor
rtPolyDataObject::~rtPolyDataObject() {
  if (m_cineFrame && m_cineFrame->isActive()) {
    m_cineFrame->stop();
  }
  if (m_cineFrame) delete m_cineFrame;

  cleanupGUI();
  clearAllData();
}
  
vtkPolyData* rtPolyDataObject::getPolyData() { 
  if (m_currentPhase==-1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get poly data. Current phase is -1."));
    return NULL;
  }
  return m_polyData[m_currentPhase];
}

vtkProperty* rtPolyDataObject::getProperty() { 
  if (m_currentPhase==-1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get property. Current phase is -1."));
    return NULL;
  }
  return m_polyProperty[m_currentPhase]; 
}

vtkColorTransferFunction* rtPolyDataObject::getColorTable() { 
  if (m_currentPhase==-1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get color table. Current phase is -1."));
    return NULL;
  }
  return m_colorLookup[m_currentPhase]; 
}
  
void rtPolyDataObject::clearAllData() {
  m_currentPhase = -1;
  m_trigDelayList.clear();
  while (!m_polyData.empty()) {
  	m_polyData.takeFirst()->Delete();
  }
  while (!m_polyProperty.empty()) {
  	m_polyProperty.takeFirst()->Delete();
  }
  while (!m_colorLookup.empty()) {
  	m_colorLookup.takeFirst()->Delete();
  }
}
  

bool rtPolyDataObject::setNewGeometry(QList<rtBasic3DPointData> *pts, QList<PolyPointLink> *links, int trigDelay) {
	
  // Check that the pointers are valid. 
  if (!pts || !links) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Pointers to new geometry are NULL."));
  	return false;
  }
  
  // Check that both lists have something in them.
  if (pts->empty() || links->empty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("At least one new geometry list is empty."));
  	return false;
  }
  
  // Check that trigger delay is within bounds. 
  if (trigDelay < 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Invalid trigger delay value: ").append(QString::number(trigDelay)));
  	return false;
  }
  
  int addIndex;
  addIndex = m_trigDelayList.indexOf(trigDelay);
  
  // Check if the trig delay does not exist.
  if (addIndex == -1) {
    // No such delay. So add it.
    m_trigDelayList.append(trigDelay);
    m_polyData.append( vtkPolyData::New() );
    vtkProperty* pr = vtkProperty::New();
    m_polyProperty.append( pr );
    m_colorLookup.append( vtkColorTransferFunction::New() );

    if(m_optionsWidget.frameSlider) m_optionsWidget.frameSlider->setMinimum(0);
    if(m_optionsWidget.frameSlider) m_optionsWidget.frameSlider->setMaximum(m_trigDelayList.count()-1);
  }
  
  if (m_currentPhase == -1) m_currentPhase = 0;

  addIndex = m_trigDelayList.indexOf(trigDelay);
  if (addIndex == -1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to add index for a new trigger delay!"));
  	return false;
  }
  
  vtkPoints *pointList = vtkPoints::New();
  vtkIntArray *scalars = vtkIntArray::New();
  vtkCellArray *surf = vtkCellArray::New();
  vtkIdType tempLine[3];

  QHash<QString, int> colorTable;
  QColor temp;

  // Remove old table points
  m_colorLookup[addIndex]->RemoveAllPoints();

  pointList->SetNumberOfPoints( pts->count() );
  scalars->SetNumberOfValues( pts->count() );
  for (int ix1=0; ix1<pts->count(); ix1++) {
    pointList->SetPoint(ix1, pts->value(ix1).getX(), pts->value(ix1).getY(), pts->value(ix1).getZ() );
    temp = pts->value(ix1).getColor();
    if (!colorTable.contains(temp.name())) {
      colorTable.insert(temp.name(), ix1);
    }
    m_colorLookup[addIndex]->AddRGBPoint(colorTable.value(temp.name()), temp.redF(), temp.greenF(), temp.blueF() );
    scalars->SetTupleValue(ix1, &colorTable[temp.name()]);
  }

  m_polyData[addIndex]->Reset();
  m_polyData[addIndex]->SetPoints(pointList);
  m_polyData[addIndex]->GetPointData()->SetScalars(scalars);
  for (int ix1=0; ix1<links->count(); ix1++) {
    tempLine[0] =  (*links)[ix1].threeVertex[0];
    tempLine[1] =  (*links)[ix1].threeVertex[1];
    tempLine[2] =  (*links)[ix1].threeVertex[2];
    surf->InsertNextCell(3, tempLine);
  }

  m_polyData[addIndex]->SetPolys(surf);

  surf->Delete();
  scalars->Delete();
  pointList->Delete();

  return true;
}

//! Copy a new poly data object. The old data is replaced.
bool rtPolyDataObject::copyPolyData(vtkPolyData* polyData) {
  if (!polyData) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not copy poly data. Pointer is NULL."));
    return false;
  }
  m_polyData[m_currentPhase]->DeepCopy(polyData);
  return true;
}

//! Copy a new lookup table. The old table is replaced.
bool rtPolyDataObject::copyLookupTable(vtkColorTransferFunction* lookupTable) {
  if (!lookupTable) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not copy poly data. Pointer is NULL."));
    return false;
  }
  m_colorLookup[m_currentPhase]->DeepCopy(lookupTable);
  return true;
}
  
bool rtPolyDataObject::setCurrTrigDelay(int trigDelay) {
  if (trigDelay < 0 || m_trigDelayList.count() == 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not set trigger delay. Delay is negative or the list is empty."));
    return false;
  }
  double diff = (m_trigDelayList[0] - trigDelay)*(m_trigDelayList[0] - trigDelay);
  int phase = 0;
  for (int ix1=1; ix1<m_trigDelayList.count(); ix1++) {
    if ( diff > (m_trigDelayList[ix1] - trigDelay)*(m_trigDelayList[ix1] - trigDelay) ) {
  		diff = (m_trigDelayList[ix1] - trigDelay)*(m_trigDelayList[ix1] - trigDelay);
  		phase = ix1;
  	}
  }
  if (m_currentPhase != phase) {
    m_currentPhase = phase;
    m_optionsWidget.frameSlider->setValue(m_currentPhase);
    Modified();
  }
  return true;
}

bool rtPolyDataObject::setCurrPhase(int phase) {
  if (phase < 0 || phase >= m_trigDelayList.count()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not set phase. Parameter is negative or phase falls otside the trigger list."));
    return false;
  }

  if (m_currentPhase != phase) {
    m_currentPhase = phase;
    m_optionsWidget.frameSlider->setValue(m_currentPhase);
    Modified();
  }
  return true;
}
  
//! Send the info to the GUI
void rtPolyDataObject::update() {

}

bool rtPolyDataObject::saveFile(QFile *file) {
  if (!file->open(QIODevice::WriteOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for writing. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  QXmlStreamWriter writer(file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeStartElement("VurtigoFile");
  rtDataObject::saveHeader(&writer, getObjectType(), getObjName());
  rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(this->getId());
  rObj->saveVisibilities(&writer);

  writer.writeTextElement("numPhases", QString::number(m_trigDelayList.size()));

  double pt[3], color[3];
  for (int ix1=0; ix1<m_trigDelayList.size(); ix1++) {
    writer.writeStartElement("Phase");
    writer.writeAttribute( "Number", QString::number(ix1) );
    writer.writeAttribute( "Trigger", QString::number(m_trigDelayList.at(ix1)) );

    writer.writeStartElement("PointList");
    for (int ix2=0; ix2<m_polyData.at(ix1)->GetNumberOfPoints(); ix2++) {
      m_polyData.at(ix1)->GetPoint(ix2, pt);
      double tupleValue;
      m_polyData.at(ix1)->GetPointData()->GetScalars()->GetTuple(ix2, &tupleValue);
      m_colorLookup.at(ix1)->GetColor(tupleValue, color);
      writer.writeStartElement("Point");
      writer.writeAttribute( "X", QString::number(pt[0]) );
      writer.writeAttribute( "Y", QString::number(pt[1]) );
      writer.writeAttribute( "Z", QString::number(pt[2]) );
      writer.writeAttribute( "R", QString::number(color[0]) );
      writer.writeAttribute( "G", QString::number(color[1]) );
      writer.writeAttribute( "B", QString::number(color[2]) );
      writer.writeAttribute( "ID", QString::number(ix2) );
      writer.writeEndElement();
    }
    writer.writeEndElement(); // End of PointList

    writer.writeStartElement("CellList");
    for (int ix2=0; ix2<m_polyData.at(ix1)->GetNumberOfCells(); ix2++) {

      // Check that there are truly three points in this polygon.
      if (m_polyData.at(ix1)->GetCell(ix2)->GetNumberOfPoints() != 3) continue;

      writer.writeStartElement("CellPoint");
      writer.writeAttribute( "Cell", QString::number( ix2 ) );
      writer.writeAttribute( "P0", QString::number( m_polyData.at(ix1)->GetCell(ix2)->GetPointId(0) ) );
      writer.writeAttribute( "P1", QString::number( m_polyData.at(ix1)->GetCell(ix2)->GetPointId(1) ) );
      writer.writeAttribute( "P2", QString::number( m_polyData.at(ix1)->GetCell(ix2)->GetPointId(2) ) );
      writer.writeEndElement();

    }
    writer.writeEndElement(); // End of CellList
    writer.writeEndElement(); // End of Phase
  }

  writer.writeEndElement(); // VurtigoFile
  writer.writeEndDocument();

  file->close();
  return true;
}

bool rtPolyDataObject::loadFile(QFile *file) {
  if (!file->open(QIODevice::ReadOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for reading. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  QXmlStreamReader reader(file);
  QXmlStreamAttributes attrib;
  QString objType;
  QString objName="";
  bool valueOK;

  int phaseNumber;
  int triggerDelay;

  clearAllData();

  while (!reader.atEnd()) {
    if (reader.readNext() == QXmlStreamReader::StartElement ) {
      if (reader.name() == "FileInfo") {
        rtDataObject::loadHeader(&reader, objType, objName);
      }
      else if (reader.name() == "Visibilities")
      {
          rtRenderObject *rObj = rtApplication::instance().getObjectManager()->getObjectWithID(this->getId());
          rObj->loadVisibilities(&reader);
      }
      else if (reader.name() == "Phase") {
        attrib = reader.attributes();
        phaseNumber = attrib.value("Number").toString().toInt(&valueOK);
        triggerDelay = attrib.value("Trigger").toString().toInt(&valueOK);
        readNewPhaseFromFile(&reader, phaseNumber, triggerDelay);
      }
    }

  }
  if (reader.hasError()) {
    return false;
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("XML Reader Failed. Error: ").append(reader.errorString()));
  }
  Modified();
  return true;
}

bool rtPolyDataObject::readNewPhaseFromFile(QXmlStreamReader* reader, int phase, int trigger) {
  if (!reader) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not read phase info from file. Reader pointer is NULL."));
    return false;
  }

  QXmlStreamAttributes attrib;
  bool valueOK;

  int id;
  QList<rtBasic3DPointData> pts;
  rtBasic3DPointData tempPT;
  QList<PolyPointLink> links;
  PolyPointLink tempLINK;

  // Start reading
  while (!reader->atEnd()) {
    if (reader->readNext() == QXmlStreamReader::StartElement ) {
      if(reader->name() == "Point") {
        attrib = reader->attributes();
        id = attrib.value("ID").toString().toInt(&valueOK);
        tempPT.setX( attrib.value("X").toString().toDouble(&valueOK) );
        tempPT.setY( attrib.value("Y").toString().toDouble(&valueOK) );
        tempPT.setZ( attrib.value("Z").toString().toDouble(&valueOK) );
        tempPT.setColor(attrib.value("R").toString().toDouble(&valueOK), attrib.value("G").toString().toDouble(&valueOK), attrib.value("B").toString().toDouble(&valueOK));
        pts.insert(id, tempPT);
      } else if (reader->name() == "CellPoint") {
        attrib = reader->attributes();
        id = attrib.value("Cell").toString().toInt(&valueOK);
        tempLINK.threeVertex[0] = attrib.value("P0").toString().toInt(&valueOK);
        tempLINK.threeVertex[1] = attrib.value("P1").toString().toInt(&valueOK);
        tempLINK.threeVertex[2] = attrib.value("P2").toString().toInt(&valueOK);
        links.insert(id, tempLINK);
      }
    } else if (reader->isEndElement() && reader->name() == "Phase") {
      // End of this phase!
      break;
    }
  }
  if (reader->hasError()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("XML Reader Failed. Error: ").append(reader->errorString()));
    return false;
  }
  setNewGeometry(&pts, &links, trigger);

  return true;
}

/////////////////
// Slots
//////////////
void rtPolyDataObject::opacityChanged(int opacity) {
  m_optionsWidget.opacityLabel->setText(QString::number(opacity).append(" %"));
  for (int ix1=0; ix1<m_polyProperty.count(); ix1++) {
    m_polyProperty[ix1]->SetOpacity(((double)opacity)/100.0f);
  }
  Modified();
}

void rtPolyDataObject::meshCheckBoxChanged(int state) {

  if (state == Qt::Checked) {
    for (int ix1=0; ix1<m_polyProperty.count(); ix1++) {
      m_polyProperty[ix1]->SetRepresentationToWireframe();
    }
  } else {
    for (int ix1=0; ix1<m_polyProperty.count(); ix1++) {
      m_polyProperty[ix1]->SetRepresentationToSurface();
    }
  }

  Modified();
}

void rtPolyDataObject::setVisibleComponent(int phase) {
  if (m_currentPhase != phase) {
    m_currentPhase = phase;
    m_optionsWidget.frameSlider->setValue(m_currentPhase);
    Modified();
  }
}


void rtPolyDataObject::nextVisibleComponent() {
  if( m_trigDelayList.count() <= 1) return;
  int trig = rtApplication::instance().getTimeManager()->getTriggerDelay();
  setCurrTrigDelay(trig);
}

void rtPolyDataObject::cineLoop(bool cine) {
  if (cine) {
    m_cineFrame->start(20);
  } else if (m_cineFrame->isActive()) {
    m_cineFrame->stop();
  }
}

////////////////
// Protected
////////////////
//! Set the GUI widgets.
void rtPolyDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  connect(m_optionsWidget.opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(opacityChanged(int)));
  connect(m_optionsWidget.renderMeshCheckBox, SIGNAL(stateChanged(int)), this, SLOT(meshCheckBoxChanged(int)));

  // Cine controls
  connect(m_optionsWidget.frameSlider, SIGNAL(valueChanged(int)), this, SLOT(setVisibleComponent(int)));
  connect(m_optionsWidget.cineLoopPushButton, SIGNAL(toggled(bool)), this, SLOT(cineLoop(bool)));
}

//! Clean the GUI widgets.
void rtPolyDataObject::cleanupGUI() {

}
