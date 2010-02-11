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
#include "rtPolyDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"

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
  setObjectType(rtConstants::OT_vtkPolyData);


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
  if (m_currentPhase==-1) return NULL;
  return m_polyData[m_currentPhase];
}

vtkProperty* rtPolyDataObject::getProperty() { 
  if (m_currentPhase==-1) return NULL;
  return m_polyProperty[m_currentPhase]; 
}

vtkColorTransferFunction* rtPolyDataObject::getColorTable() { 
  if (m_currentPhase==-1) return NULL;
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
  

bool rtPolyDataObject::setNewGeometry(QList<PolyPoint> *pts, QList<PolyPointLink> *links, double trigDelay) {
	
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
    if (m_currentPhase == -1) m_currentPhase = 0;
    m_polyData.append( vtkPolyData::New() );
    vtkProperty* pr = vtkProperty::New();
    m_polyProperty.append( pr );
    m_colorLookup.append( vtkColorTransferFunction::New() );

    if(m_optionsWidget.frameSlider) m_optionsWidget.frameSlider->setMinimum(0);
    if(m_optionsWidget.frameSlider) m_optionsWidget.frameSlider->setMaximum(m_trigDelayList.count()-1);
  }
  
  addIndex = m_trigDelayList.indexOf(trigDelay);
  if (addIndex == -1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to add index for a new trigger delay!"));
  	return false;
  }
  
  vtkPoints *pointList = vtkPoints::New();
  vtkIntArray *scalars = vtkIntArray::New();
  vtkCellArray *surf = vtkCellArray::New();
  int tempLine[3];

  QHash<QString, int> colorTable;
  QColor temp;

  // Remove old table points
  m_colorLookup[addIndex]->RemoveAllPoints();

  pointList->SetNumberOfPoints( pts->count() );
  scalars->SetNumberOfValues( pts->count() );
  for (int ix1=0; ix1<pts->count(); ix1++) {
    pointList->SetPoint(ix1, pts->value(ix1).ptList);
    temp.setRed(pts->value(ix1).color[0]);
    temp.setGreen(pts->value(ix1).color[1]);
    temp.setBlue(pts->value(ix1).color[2]);
    if (!colorTable.contains(temp.name())) {
      colorTable.insert(temp.name(), ix1);
    }
    m_colorLookup[addIndex]->AddRGBPoint(colorTable.value(temp.name()), ((double)temp.red())/255.0f, ((double)temp.green())/255.0f, ((double)temp.blue())/255.0f );
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
  if (!polyData) return false;
  m_polyData[m_currentPhase]->DeepCopy(polyData);
  return true;
}

//! Copy a new lookup table. The old table is replaced.
bool rtPolyDataObject::copyLookupTable(vtkColorTransferFunction* lookupTable) {
  if (!lookupTable) return false;
  m_colorLookup[m_currentPhase]->DeepCopy(lookupTable);
  return true;
}
  
bool rtPolyDataObject::setCurrTrigDelay(double trigDelay) {
  if (trigDelay < 0 || m_trigDelayList.count() == 0) return false;
  double diff = (m_trigDelayList[0] - trigDelay)*(m_trigDelayList[0] - trigDelay);
  int phase = 0;
  for (int ix1=1; ix1<m_trigDelayList.count(); ix1++) {
  	if ( diff > (m_trigDelayList[ix1] - trigDelay)*(m_trigDelayList[ix1] - trigDelay) ) {
  		diff = (m_trigDelayList[ix1] - trigDelay)*(m_trigDelayList[ix1] - trigDelay);
  		phase = ix1;
  	}
  }
  m_currentPhase = phase;
  return true;
}

bool rtPolyDataObject::setCurrPhase(int phase) {
  if (phase < 0 || phase >= m_trigDelayList.count()) return false;
  m_currentPhase = phase;
  return true;
}
  
//! Send the info to the GUI
void rtPolyDataObject::update() {

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

void rtPolyDataObject::setVisibleComponent(int comp) {
  m_currentPhase = comp;
  Modified();
}


void rtPolyDataObject::nextVisibleComponent() {
  if( m_trigDelayList.count() <= 1) return;
  m_currentPhase = (m_currentPhase + 1) % m_trigDelayList.count();
  m_optionsWidget.frameSlider->setValue(m_currentPhase);
  Modified();
}

void rtPolyDataObject::cineLoop(bool cine) {
  if (cine) {
    m_cineFrame->start(100);
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
