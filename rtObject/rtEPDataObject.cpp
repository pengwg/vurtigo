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
#include "rtEPDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

#include <QHash>
#include <QColor>
#include <QString>
#include <QThread>
#include <QThreadPool>
#include <QApplication>

#include <vtkPoints.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

//! Constructor
rtEPDataObject::rtEPDataObject()
    : m_currentPhase(-1),  m_objTransform(0), m_inPlaneInterval(1.0), m_crossPlaneInterval(1.0), m_surfaceOpacity(1.0), m_pointsOpacity(1.0), m_rep(EP_SURFACE),
    m_minSliceNum(0), m_maxSliceNum(0), m_showInfoPoints(false)
{
  setObjectType(rtConstants::OT_EPMesh);

  m_phaseDataList.clear();

  m_objTransform = vtkTransform::New();

  m_threadCount = QThread::idealThreadCount();
  if (m_threadCount == -1) m_threadCount = 1;

  setupGUI();


  ///// TEST ///////////
  rtEPPropertyPointList::InfoPoint p;
  p.location[0] = -9.5;
  p.location[1] = -38.0;
  p.location[2] = 84.2;
  p.property = 15;
  addInfoPoint(p, "TEST");

  p.location[0] = -9.5;
  p.location[1] = -38.0;
  p.location[2] = 85.2;
  p.property = 15;
  addInfoPoint(p, "TEST");

  p.location[0] = -9.5;
  p.location[1] = -38.0;
  p.location[2] = 89.2;
  p.property = 5;
  addInfoPoint(p, "TEST");

  p.location[0] = -10.5;
  p.location[1] = -38.0;
  p.location[2] = 87.2;
  p.property = 7;
  addInfoPoint(p, "TEST");
  ////////// END TEST ///////////
}

//! Destructor
rtEPDataObject::~rtEPDataObject() {
  cleanupGUI();

  QList<int> listKeys = m_phaseDataList.keys();
  for (int ix1=0; ix1<listKeys.size(); ix1++) {
    deletePhase(listKeys.at(ix1));
  }

  if (m_objTransform) {
    m_objTransform->Delete();
    m_objTransform = NULL;
  }
}

bool rtEPDataObject::setTransform(vtkTransform* t) {
  if (!t) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("The new transform t is NULL."));
    return false;
  }
  if (!m_objTransform) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("The object transform m_objTransform is NULL."));
    return false;
  }

  m_objTransform->DeepCopy(t);
  Modified();
  return true;
}

bool rtEPDataObject::setTriggerDelay(int phase, int trigger) {
  if(phase < 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Phase is negative."));
    return false;
  }

  if(trigger < 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Trigger is negative."));
    return false;
  }

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);
  m_phaseDataList[phase].triggerDelay = trigger;
  m_cineWidget.addTrigger(trigger);
  Modified();
  return true;
}

bool rtEPDataObject::setCurrTrigDelay(int trigDelay) {
  if (trigDelay < 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot set a negative trigger delay."));
    return false;
  }

  QList<int> listKeys = m_phaseDataList.keys();

  int phase = 0;
  int diff = abs(trigDelay - m_phaseDataList.value(listKeys.value(0)).triggerDelay);
  // Find the closest trigger dealy and set the phase

  for (int ix1=1; ix1<listKeys.size(); ix1++) {
    if (diff > abs(trigDelay - m_phaseDataList.value(listKeys.value(ix1)).triggerDelay)) {
      phase = listKeys.value(ix1);
      diff = abs(trigDelay - m_phaseDataList.value(listKeys.value(ix1)).triggerDelay);
    }
  }
  m_currentPhase = phase;
  Modified();
  return true;
}

bool rtEPDataObject::setCurrPhase(int phase) {
  if(phase < 0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot set a negative phase."));
    return false;
  }

  if(!m_phaseDataList.contains(phase)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Phase list does not contain this phase."));
    return false;
  }

  m_currentPhase = phase;
  Modified();
  return true;
}


bool rtEPDataObject::phaseExists(int phaseNum) {
  return m_phaseDataList.contains(phaseNum);
}

void rtEPDataObject::createPhase(int phaseNum) {
  if (phaseNum<0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot create a negative phase."));
    return;
  }

  rtApplication::instance().getMessageHandle()->debug(QString("Creating phase number: ").append(QString::number(phaseNum)) );

  PhaseData dat;

  dat.triggerDelay = 0;
  dat.pointList.clear();
  dat.pointData = vtkPolyData::New();
  dat.pointData->SetPoints(vtkPoints::New());
  dat.pointData->SetVerts(vtkCellArray::New());
  dat.pointProperty = vtkProperty::New();

  dat.pointDataUpdate = true;

  for (int ix1=0; ix1<3; ix1++) {
    dat.sliceSpline[ix1].clear();
    dat.posSpline[ix1].clear();
  }

  dat.meshData = vtkPolyData::New();
  dat.meshData->SetPoints(vtkPoints::New());
  dat.meshData->SetPolys(vtkCellArray::New());
  dat.meshProperty = vtkProperty::New();
  dat.meshDataUpdate = true;

  m_phaseDataList.insert(phaseNum, dat);

  if(m_currentPhase < 0) m_currentPhase = phaseNum;

  rtApplication::instance().getMessageHandle()->debug(QString("Finished creating phase number: ").append(QString::number(phaseNum)) );

  Modified();
}

void rtEPDataObject::deletePhase(int phaseNum) {
  if (phaseNum<0) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot delete a negative phase."));
    return;
  }

  rtApplication::instance().getMessageHandle()->debug(QString("Deleting phase number: ").append(QString::number(phaseNum)) );

  PhaseData dat = m_phaseDataList.take(phaseNum);

  dat.triggerDelay = 0;
  dat.pointList.clear();
  if(dat.pointData) {
    dat.pointData->Delete();
    dat.pointData = NULL;
  }

  if(dat.pointProperty) {
    dat.pointProperty->Delete();
    dat.pointProperty = NULL;
  }

  dat.pointDataUpdate = false; // no need for updates

  cleanupSliceSpline(&dat);
  cleanupPositionSpline(&dat);

  if(dat.meshData) {
    dat.meshData->Delete();
    dat.meshData = NULL;
  }

  if(dat.meshProperty) {
    dat.meshProperty->Delete();
    dat.meshProperty = NULL;
  }

  dat.meshDataUpdate = false; // no need for updates

  rtApplication::instance().getMessageHandle()->debug(QString("Finished deleting phase number: ").append(QString::number(phaseNum)) );

  Modified();
}


//! Send the info to the GUI
void rtEPDataObject::update() {

}

bool rtEPDataObject::addPoint(int phase, int slice, rtEPDataObject::EPPoint pt) {
  if(phase < 0 || slice < 0) return false;

  if (slice < m_minSliceNum) {
    m_minSliceNum = slice;
    m_optionsWidget.minSliceSlider->setMinimum(m_minSliceNum);
    m_optionsWidget.maxSliceSlider->setMinimum(m_minSliceNum);
    m_optionsWidget.minSliceSlider->setValue(m_minSliceNum);
  }

  if (slice > m_maxSliceNum) {
    m_maxSliceNum = slice;
    m_optionsWidget.minSliceSlider->setMaximum(m_maxSliceNum);
    m_optionsWidget.maxSliceSlider->setMaximum(m_maxSliceNum);
    m_optionsWidget.maxSliceSlider->setValue(m_maxSliceNum);
  }

  // Create the phase if it does not exist.
  if (!phaseExists(phase)) createPhase(phase);

  m_phaseDataList[phase].pointList.insert(slice, pt);
  m_phaseDataList[phase].pointDataUpdate = true;
  m_phaseDataList[phase].meshDataUpdate = true;
  Modified();
  return true;
}

QList<rtEPDataObject::EPPoint> rtEPDataObject::getPoints(int phase, int slice) {
  return m_phaseDataList.value(phase).pointList.values(slice);
}


vtkPolyData* rtEPDataObject::getPointData() {
  if (phaseExists(m_currentPhase)) {
    updatePointData(m_currentPhase);
    return m_phaseDataList.value(m_currentPhase).pointData;
  } else {
    return NULL;
  }
}

vtkProperty* rtEPDataObject::getPointProperty() {
  if (phaseExists(m_currentPhase)) {
    updatePointProperty();
    return m_phaseDataList.value(m_currentPhase).pointProperty;
  } else {
    return NULL;
  }
}

vtkPolyData* rtEPDataObject::getMeshData() {
  if (phaseExists(m_currentPhase)) {
    updateMeshData(m_currentPhase);
    return m_phaseDataList.value(m_currentPhase).meshData;
  } else {
    return NULL;
  }
}

vtkProperty* rtEPDataObject::getMeshProperty() {
  if (phaseExists(m_currentPhase)) {
    updateMeshProperty();
    return m_phaseDataList.value(m_currentPhase).meshProperty;
  } else {
    return NULL;
  }
}


void rtEPDataObject::addInfoPoint(rtEPPropertyPointList::InfoPoint point, QString name) {
  m_EPInfoObject.addInfoPoint(point, name);
  int numElem = m_optionsWidget.colorByComboBox->count();
  bool found = false;

  // Check if the item exists
  for (int ix1=0; ix1<numElem; ix1++) {
    if (m_optionsWidget.colorByComboBox->itemText(ix1) == name) found = true;
  }
  // Add it if the item does not exist.
  if (!found) m_optionsWidget.colorByComboBox->insertItem(numElem, name);
}

bool rtEPDataObject::saveFile(QFile *file) {
  if (!file->open(QIODevice::WriteOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for writing. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  // TODO ***

  file->close();
  return true;
}

bool rtEPDataObject::loadFile(QFile *file) {
  if (!file->open(QIODevice::ReadOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for reading. Error Code: ").append(QString::number(file->error())));
    return false;
  }

  // TODO ***

  Modified();
  return true;
}

///////////////
// Slots
///////////////
void rtEPDataObject::surfaceOpacityChanged(int val) {
  m_optionsWidget.surfaceOpacityLabel->setText(QString::number(val).append(" %"));
  m_surfaceOpacity = ((double)val)/100.0f;
  Modified();
}

void rtEPDataObject::pointsOpacityChanged(int val) {
  m_optionsWidget.pointsOpacityLabel->setText(QString::number(val).append(" %"));
  m_pointsOpacity = ((double)val)/100.0f;
  Modified();
}

void rtEPDataObject::triggerChanged(int trig) {
  QList<int> k = m_phaseDataList.keys();
  QList<PhaseData> v = m_phaseDataList.values();

  int dist = abs(trig-v[0].triggerDelay);
  int phase = k[0];
  for (int ix1=1; ix1<v.size(); ix1++) {
    if (abs(trig-v[ix1].triggerDelay) < dist) {
      dist = abs(trig-v[ix1].triggerDelay);
      phase = k[ix1];
    }
  }
  setCurrPhase(phase);
  Modified();
}

void rtEPDataObject::minSliceChanged(int val) {

  if(val == m_optionsWidget.minSliceSlider->maximum()) {
    m_optionsWidget.minSliceSlider->setValue(val-1);
    return;
  }

  m_optionsWidget.minSliceLabel->setText(QString::number(val));
  if (m_optionsWidget.maxSliceSlider->value() <= val)
    m_optionsWidget.maxSliceSlider->setValue(val+1);

  setModifyFlagForAll();
  Modified();
}

void rtEPDataObject::maxSliceChanged(int val) {

  if (val == m_optionsWidget.maxSliceSlider->minimum()) {
    m_optionsWidget.maxSliceSlider->setValue(val+1);
    return;
  }

  m_optionsWidget.maxSliceLabel->setText(QString::number(val));
  if (m_optionsWidget.minSliceSlider->value() >= val)
    m_optionsWidget.minSliceSlider->setValue(val-1);

  setModifyFlagForAll();
  Modified();
}

void rtEPDataObject::updateObjectNow() {
  QList<int> phaseList = m_phaseDataList.keys();

  m_optionsWidget.applyProgressBar->setRange(0, phaseList.size());
  m_optionsWidget.applyProgressBar->reset();
  QApplication::instance()->processEvents();
  for (int ix1=0; ix1<phaseList.size(); ix1++) {
    updateMeshData(phaseList.at(ix1));
    updatePointData(phaseList.at(ix1));
    m_optionsWidget.applyProgressBar->setValue(ix1);
    QApplication::instance()->processEvents();
  }
  m_optionsWidget.applyProgressBar->setValue(phaseList.size());
}

void rtEPDataObject::representationChanged(int val) {
  if(val == -1) return;

  EPSurfaceRep oldRep = m_rep;

  switch (val) {
    case EP_SURFACE:
    m_rep = EP_SURFACE;
    break;
    case EP_WIREFRAME:
    m_rep = EP_WIREFRAME;
    break;
    case EP_POINTS:
    m_rep = EP_POINTS;
    break;
    default:
    break;
  }
  if (oldRep != m_rep) Modified();
}

void rtEPDataObject::inSliceValueChanged(double val) {
  m_inPlaneInterval = val;
  setModifyFlagForAll();
  Modified();
}

void rtEPDataObject::betweenSliceValueChanged(double val) {
  m_crossPlaneInterval = val;
  setModifyFlagForAll();
  Modified();
}

void rtEPDataObject::colorByPropertyChanged(int comboLoc) {
  // Check that an option is actually selected.
  if (comboLoc == -1) return;

  m_EPInfoObject.setCurrentPropName(m_optionsWidget.colorByComboBox->itemText(comboLoc));
}

void rtEPDataObject::effectSizeChanged(int effect) {
  if (effect <= 0 ) return;
  m_EPInfoObject.setEffectRadius((unsigned int)effect);
  setModifyFlagForAll();
  Modified();
}

void rtEPDataObject::showPointsChanged(bool show) {
  m_showInfoPoints = show;
  Modified();
}

////////////////
// Protected
////////////////
//! Set the GUI widgets.
void rtEPDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  // The cine widget
  m_optionsWidget.epTabs->widget(1)->layout()->addWidget(&m_cineWidget);

  m_optionsWidget.surfaceOpacitySlider->setValue(100);
  m_optionsWidget.pointsOpacitySlider->setValue(100);

  m_optionsWidget.surfaceOpacityLabel->setText("100 %");
  m_optionsWidget.pointsOpacityLabel->setText("100 %");

  m_optionsWidget.repComboBox->insertItem(EP_SURFACE, "Surface");
  m_optionsWidget.repComboBox->insertItem(EP_WIREFRAME, "Wireframe");
  m_optionsWidget.repComboBox->insertItem(EP_POINTS, "Points");
  m_optionsWidget.repComboBox->setCurrentIndex(EP_SURFACE);

  connect(m_optionsWidget.surfaceOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(surfaceOpacityChanged(int)));
  connect(m_optionsWidget.pointsOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(pointsOpacityChanged(int)));

  connect(m_optionsWidget.repComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(representationChanged(int)));

  connect(m_optionsWidget.minSliceSlider, SIGNAL(valueChanged(int)), this, SLOT(minSliceChanged(int)));
  connect(m_optionsWidget.maxSliceSlider, SIGNAL(valueChanged(int)), this, SLOT(maxSliceChanged(int)));
  connect(m_optionsWidget.applyAllNow, SIGNAL(clicked()), this, SLOT(updateObjectNow()));

  connect(&m_cineWidget, SIGNAL(triggerChanged(int)), this, SLOT(triggerChanged(int)));

  connect(m_optionsWidget.inSliceSpace, SIGNAL(valueChanged(double)), this, SLOT(inSliceValueChanged(double)));
  connect(m_optionsWidget.betweenSliceSpace, SIGNAL(valueChanged(double)), this, SLOT(betweenSliceValueChanged(double)));

  connect(m_optionsWidget.colorByComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(colorByPropertyChanged(int)));
  connect(m_optionsWidget.effectSlider, SIGNAL(valueChanged(int)), this, SLOT(effectSizeChanged(int)));
  connect(m_optionsWidget.showPointsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showPointsChanged(bool)));

}

//! Clean the GUI widgets.
void rtEPDataObject::cleanupGUI() {

}

void rtEPDataObject::cleanupSliceSpline(PhaseData* data) {
  if (!data) return;

  QList<vtkKochanekSpline*> tempList;
  for (int coord = 0; coord<3; coord++) {
    tempList =  data->sliceSpline[coord].values();
    while (!tempList.empty()) {
      vtkKochanekSpline* spline = tempList.takeFirst();
      if(spline) spline->Delete();
    }
    data->sliceSpline[coord].clear();
  }
}

void rtEPDataObject::cleanupPositionSpline(PhaseData* data) {
  if (!data) return;

  QList<vtkKochanekSpline*> tempList;
  for (int coord = 0; coord<3; coord++) {
    tempList =  data->posSpline[coord].values();
    while (!tempList.empty()) {
      vtkKochanekSpline* spline = tempList.takeFirst();
      if(spline) spline->Delete();
    }
    data->posSpline[coord].clear();
  }
}

void rtEPDataObject::updatePointData(int updatePhase) {

  if (!phaseExists(updatePhase)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot update point data. Phase does not exist: ").append(QString::number(updatePhase)));
    return;
  }

  rtApplication::instance().getMessageHandle()->debug(QString("Start Updating Point Data"));
  if (m_phaseDataList[updatePhase].pointDataUpdate) {
    vtkPoints* pts = m_phaseDataList[updatePhase].pointData->GetPoints();
    vtkCellArray* cells = m_phaseDataList[updatePhase].pointData->GetVerts();
    QList<EPPoint> tempPT;
    vtkIdType pID;

    // Clear the previous lists.
    pts->Reset();
    cells->Reset();
    rtApplication::instance().getMessageHandle()->debug(QString("Cleared Previous Point Data Lists"));
    QList<int> slices = m_phaseDataList.value(updatePhase).pointList.uniqueKeys();

    for (int ix1=0; ix1<slices.size(); ix1++) {
      if (slices[ix1] < m_optionsWidget.minSliceSlider->value() || slices[ix1] > m_optionsWidget.maxSliceSlider->value()) continue;

      tempPT = m_phaseDataList.value(updatePhase).pointList.values(slices[ix1]);
      for (int ix2=0; ix2<tempPT.size(); ix2++) {
        pID = pts->InsertNextPoint(tempPT.at(ix2).x, tempPT.at(ix2).y, tempPT.at(ix2).z);
        cells->InsertNextCell(1, &pID);
      }
    }

    m_phaseDataList[updatePhase].pointData->SetPoints(pts);
    m_phaseDataList[updatePhase].pointData->SetVerts(cells);
    m_phaseDataList[updatePhase].pointData->Modified();
    m_phaseDataList[updatePhase].pointDataUpdate = false;
  }
  rtApplication::instance().getMessageHandle()->debug(QString("Finished Updating Point Data"));
}

void rtEPDataObject::updateMeshData(int updatePhase) {

  if (!phaseExists(updatePhase)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Cannot update mesh data. Phase does not exist: ").append(QString::number(updatePhase)));
    return;
  }

  rtApplication::instance().getMessageHandle()->debug(QString("Start Updating Mesh Data"));

    if (m_phaseDataList[updatePhase].meshDataUpdate) {
      QList<EPPoint> tempPT, tempPT2;
      QList<int> slices = m_phaseDataList.value(updatePhase).pointList.uniqueKeys();
      vtkKochanekSpline *tempSpline[3];
      int maxSlice = 0;
      unsigned int maxPos = 0;

      // Cleaup the old position spline.
      cleanupPositionSpline(&(m_phaseDataList[updatePhase]));

      // Populate the slice splines first.
      for (int ix1=0; ix1<slices.size(); ix1++) {
        if (slices[ix1] < m_optionsWidget.minSliceSlider->value() || slices[ix1] > m_optionsWidget.maxSliceSlider->value()) continue;
        tempPT = m_phaseDataList[updatePhase].pointList.values(slices[ix1]);

        // Determine the maximum slice.
        if(slices[ix1] > maxSlice) maxSlice = slices[ix1];

        for (int coord=0; coord<3; coord++) {
          if (m_phaseDataList[updatePhase].sliceSpline[coord].contains(slices[ix1])) {
            tempSpline[coord] = m_phaseDataList[updatePhase].sliceSpline[coord].value(slices[ix1]);
            tempSpline[coord]->RemoveAllPoints();
          } else {
            tempSpline[coord] = vtkKochanekSpline::New();
            m_phaseDataList[updatePhase].sliceSpline[coord].insert(slices[ix1], tempSpline[coord]);
          }
        }

        for (int ix2=0; ix2<tempPT.size(); ix2++) {
          tempSpline[0]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).x);
          tempSpline[1]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).y);
          tempSpline[2]->AddPoint(tempPT.at(ix2).loc, tempPT.at(ix2).z);

          // Determine the maximum position.
          if (tempPT.at(ix2).loc > maxPos) maxPos = tempPT.at(ix2).loc;
        }
      }

      for (int coord=0; coord<3; coord++) {
        CreatePositionSplinesTask* temp = new CreatePositionSplinesTask(&m_phaseDataList[updatePhase], coord, maxPos, m_inPlaneInterval, &slices, m_optionsWidget.minSliceSlider->value(), m_optionsWidget.maxSliceSlider->value());
        QThreadPool::globalInstance()->start(temp);
      }
      QThreadPool::globalInstance()->waitForDone();

      vtkPoints* pts = m_phaseDataList[updatePhase].meshData->GetPoints();
      vtkCellArray* cells = m_phaseDataList[updatePhase].meshData->GetPolys();

      pts->Reset();
      cells->Reset();

      vtkIdType* pID = new vtkIdType[4];

      double xyzCoords[3];
      double height = 0.0;
      double pos = 0.0;
      for (height=0.0; height<maxSlice; height += m_crossPlaneInterval) {

        double crossInterval;
        if (height+m_crossPlaneInterval > maxSlice) {
          crossInterval = maxSlice - height;
        } else {
          crossInterval = m_crossPlaneInterval;
        }

        for (pos=0.0; pos<maxPos; pos+=m_inPlaneInterval) {

          double planeInterval;
          if (pos+m_inPlaneInterval > maxPos) {
            planeInterval = maxPos - pos;
          } else {
            planeInterval = m_inPlaneInterval;
          }

          xyzCoords[0] = m_phaseDataList[updatePhase].posSpline[0].value(pos)->Evaluate(height);
          xyzCoords[1] = m_phaseDataList[updatePhase].posSpline[1].value(pos)->Evaluate(height);
          xyzCoords[2] = m_phaseDataList[updatePhase].posSpline[2].value(pos)->Evaluate(height);
          pID[0] = pts->InsertNextPoint(xyzCoords);

          if (pID[0] < 0) {
            rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to insert next point at 0: ").append(QString::number(pos)));
          }

          xyzCoords[0] = m_phaseDataList[updatePhase].posSpline[0].value(pos)->Evaluate(height+crossInterval);
          xyzCoords[1] = m_phaseDataList[updatePhase].posSpline[1].value(pos)->Evaluate(height+crossInterval);
          xyzCoords[2] = m_phaseDataList[updatePhase].posSpline[2].value(pos)->Evaluate(height+crossInterval);
          pID[1] = pts->InsertNextPoint(xyzCoords);

          if (pID[1] < 0) {
            rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to insert next point at 1: ").append(QString::number(pos)));
          }

          xyzCoords[0] = m_phaseDataList[updatePhase].posSpline[0].value(pos+planeInterval)->Evaluate(height+crossInterval);
          xyzCoords[1] = m_phaseDataList[updatePhase].posSpline[1].value(pos+planeInterval)->Evaluate(height+crossInterval);
          xyzCoords[2] = m_phaseDataList[updatePhase].posSpline[2].value(pos+planeInterval)->Evaluate(height+crossInterval);
          pID[2] = pts->InsertNextPoint(xyzCoords);

          if (pID[2] < 0) {
            rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to insert next point at 2: ").append(QString::number(pos)));
          }

          xyzCoords[0] = m_phaseDataList[updatePhase].posSpline[0].value(pos+planeInterval)->Evaluate(height);
          xyzCoords[1] = m_phaseDataList[updatePhase].posSpline[1].value(pos+planeInterval)->Evaluate(height);
          xyzCoords[2] = m_phaseDataList[updatePhase].posSpline[2].value(pos+planeInterval)->Evaluate(height);
          pID[3] = pts->InsertNextPoint(xyzCoords);

          if (pID[3] < 0) {
            rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to insert next point at 3: ").append(QString::number(pos)));
          }

          cells->InsertNextCell(4, pID);
        }
      }


      rtApplication::instance().getMessageHandle()->debug(QString("Mesh Data Number of Points: ").append(QString::number(pts->GetNumberOfPoints())));
      rtApplication::instance().getMessageHandle()->debug(QString("Mesh Data Number of Cells: ").append(QString::number(cells->GetNumberOfCells())));
      rtApplication::instance().getMessageHandle()->debug(QString("Max Cell ID is: ").append(QString::number(cells->GetData()->GetMaxId())));

      // Finally, create the mesh
      m_phaseDataList[updatePhase].meshData->SetPoints(pts);
      m_phaseDataList[updatePhase].meshData->SetPolys(cells);
      m_phaseDataList[updatePhase].meshData->Modified();

      delete[] pID;

      // Update the scalar values to have the colors take effect.
      m_EPInfoObject.updateScalars(m_phaseDataList[updatePhase].meshData);

      m_phaseDataList[updatePhase].meshDataUpdate = false;
    }
  rtApplication::instance().getMessageHandle()->debug(QString("Finished Updating Mesh Data"));
}

void rtEPDataObject::updatePointProperty() {
  if (phaseExists(m_currentPhase)) {
    m_phaseDataList[m_currentPhase].pointProperty->SetOpacity(m_pointsOpacity);
  }
}

void rtEPDataObject::updateMeshProperty() {
  if (phaseExists(m_currentPhase)) {

    switch (m_rep) {
    case EP_SURFACE:
      m_phaseDataList[m_currentPhase].meshProperty->SetRepresentationToSurface();
      break;
    case EP_WIREFRAME:
      m_phaseDataList[m_currentPhase].meshProperty->SetRepresentationToWireframe();
      break;
    case EP_POINTS:
      m_phaseDataList[m_currentPhase].meshProperty->SetRepresentationToPoints();
      break;
    default:
      break;
    }

    m_phaseDataList[m_currentPhase].meshProperty->SetOpacity(m_surfaceOpacity);
  }
}

void rtEPDataObject::setModifyFlagForAll() {

  rtApplication::instance().getMessageHandle()->debug(QString("Setting Modify Flag for all phases."));

  QList<int> phaseList = m_phaseDataList.keys();

  for (int ix1=0; ix1<phaseList.size(); ix1++) {
    m_phaseDataList[phaseList.at(ix1)].meshDataUpdate = true;
    m_phaseDataList[phaseList.at(ix1)].pointDataUpdate = true;
  }

  // Reset the progress bar.
  m_optionsWidget.applyProgressBar->setRange(0, phaseList.size());
  m_optionsWidget.applyProgressBar->reset();

  rtApplication::instance().getMessageHandle()->debug(QString("All phases modified."));
}
