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
#include "CardiacMeshReaderUI.h"

#include <QFileDialog>

#include <iostream>

#include <vtkErrorCode.h>

#include "rt3dVolumeDataObject.h"
#include "rtPolyDataObject.h"
#include "rtBaseHandle.h"
#include "DICOMFileReader.h"

CardiacMeshReaderUI::CardiacMeshReaderUI() {
  setupUi(this);

  m_lastDir = "";
  m_lastMeshDir = "";

  // Buttons start disabled.
  p1FinishButton->setEnabled(false);
  p1NextButton->setEnabled(false);

  p2FinishButton->setEnabled(false);
  p2NextButton->setEnabled(false);

  connectSignals();
}

CardiacMeshReaderUI::~CardiacMeshReaderUI() {
}

void CardiacMeshReaderUI::connectSignals() {

  // Page 1
  connect(directoryEdit, SIGNAL(editingFinished()), this, SLOT(newDirectory()));
  connect(directoryChooser, SIGNAL(clicked()), this, SLOT(dirChooser()));
  connect(p1NextButton, SIGNAL(clicked()), this, SLOT(page1Next()));
  connect(p1FinishButton, SIGNAL(clicked()), this, SLOT(page1Finish()));

  // Page 2
  connect(directoryEditMesh, SIGNAL(editingFinished()), this, SLOT(newDirectoryMesh()));
  connect(directoryChooserMesh, SIGNAL(clicked()), this, SLOT(dirChooserMesh()));
  connect(p2BackButton, SIGNAL(clicked()), this, SLOT(page2Back()));
  connect(p2NextButton, SIGNAL(clicked()), this, SLOT(page2Next()));
  connect(p2FinishButton, SIGNAL(clicked()), this, SLOT(page2Finish()));

  // Page 3
  connect(nameLineEdit, SIGNAL(editingFinished()), this, SLOT(namesChanged()));
  connect(meshNameLineEdit, SIGNAL(editingFinished()), this, SLOT(namesChanged()));
  connect(p3BackButton, SIGNAL(clicked()), this, SLOT(page3Back()));
  connect(p3FinishButton, SIGNAL(clicked()), this, SLOT(page3Finish()));
}

//! Slot called when the user changes the directory.
void CardiacMeshReaderUI::newDirectory() {

  if ( m_lastDir != directoryEdit->text() ) {
    m_lastDir = directoryEdit->text();

    // Use the custom reader too.
    bool ok = m_customReader.setDirectory(m_lastDir);
    infoBrowser->clear();
    if (ok) {
      infoBrowser->append(m_customReader.getComments());
      m_customReader.createVolume(m_customReader.getDICOMImageData());
      p1FinishButton->setEnabled(true);
      p1NextButton->setEnabled(true);
      m_meshReader.setNumPhases(m_customReader.getImageData()->GetNumberOfScalarComponents());
    } else {
      infoBrowser->append("Error!");
      p1FinishButton->setEnabled(false);
      p1NextButton->setEnabled(false);
    }

  }
}

//! Popup a dialog box to choose a directory
void CardiacMeshReaderUI::dirChooser() {
  QString dir;

  dir = QFileDialog::getExistingDirectory(this, "Select DICOM Directory", "", QFileDialog::ShowDirsOnly);

  if (dir != "") {
    // The user did select a directory.
    directoryEdit->setText(dir);
    newDirectory();
  }
}

//! Go to the next set in the process.
void CardiacMeshReaderUI::page1Next() {
  meshStackedWidget->setCurrentIndex(1);
}

//! Save the current set of DICOM images as a volume
void CardiacMeshReaderUI::page1Finish() {
  m_vol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, m_customReader.getDefaultName());
  if (m_vol >=0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

    if (ptObj) {
      ptObj->lock();
      ptObj->copyNewTransform(m_customReader.getTransform());
      ptObj->copyNewImageData(m_customReader.getImageData());
      ptObj->copyTriggerDelayList(m_customReader.getTriggerList());
      ptObj->Modified();
      ptObj->unlock();
    }
  }
}

void CardiacMeshReaderUI::newDirectoryMesh() {
  if ( m_lastMeshDir != directoryEditMesh->text() ) {
    m_lastMeshDir = directoryEditMesh->text();

    // Use the custom mesh reader.
    bool ok = m_meshReader.setDirectory(m_lastMeshDir);
    infoBrowserMesh->clear();
    if (ok) {
      infoBrowserMesh->append(m_meshReader.getComments());
      p2FinishButton->setEnabled(true);
      p2NextButton->setEnabled(true);
    } else {
      infoBrowserMesh->append("Error!");
      p2FinishButton->setEnabled(false);
      p2NextButton->setEnabled(false);
    }

  }
}

void CardiacMeshReaderUI::dirChooserMesh() {
  QString dir;

  dir = QFileDialog::getExistingDirectory(this, "Select Mesh Directory", "", QFileDialog::ShowDirsOnly);

  if (dir != "") {
    // The user did select a directory.
    directoryEditMesh->setText(dir);
    newDirectoryMesh();
  }
}

void CardiacMeshReaderUI::page2Next() {
  meshStackedWidget->setCurrentIndex(2);
  nameLineEdit->setText(m_customReader.getDefaultName());
  meshNameLineEdit->setText(m_meshReader.getDefaultName());
}


void CardiacMeshReaderUI::page2Back() {
  meshStackedWidget->setCurrentIndex(0);
}

void CardiacMeshReaderUI::page2Finish() {
  // Set the auto titles and finish the job.
  nameLineEdit->setText(m_customReader.getDefaultName());
  meshNameLineEdit->setText(m_meshReader.getDefaultName());

  page3Finish();
}

void CardiacMeshReaderUI::namesChanged() {
  if (nameLineEdit->text().trimmed() == "" || meshNameLineEdit->text().trimmed() == "" ) {
    p3FinishButton->setEnabled(false);
  } else {
    p3FinishButton->setEnabled(true);
  }
}


void CardiacMeshReaderUI::page3Back() {
  meshStackedWidget->setCurrentIndex(1);
}

void CardiacMeshReaderUI::page3Finish() {

  // Create first the DICOM volume.
  m_vol = rtBaseHandle::instance().requestNewObject( rtConstants::OT_3DObject, nameLineEdit->text() );
  if (m_vol >=0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

    if (ptObj) {
      ptObj->lock();
      ptObj->copyNewTransform(m_customReader.getTransform());
      ptObj->copyNewImageData(m_customReader.getImageData());
      ptObj->copyTriggerDelayList(m_customReader.getTriggerList());
      ptObj->Modified();
      ptObj->unlock();
    }
  }

  m_icontour = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkPolyData, meshNameLineEdit->text());
  m_ocontour = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkPolyData, meshNameLineEdit->text());

  if (m_icontour>=0) {
    rtPolyDataObject* iContourObj = static_cast<rtPolyDataObject*>(rtBaseHandle::instance().getObjectWithID(m_icontour));
    if(iContourObj) {
      loadPolyDataFromPoints(iContourObj, MeshPointSet::PT_ICONTOUR);
    }
  }

  if (m_ocontour>=0) {
    rtPolyDataObject* oContourObj = static_cast<rtPolyDataObject*>(rtBaseHandle::instance().getObjectWithID(m_ocontour));
    if(oContourObj) {
      loadPolyDataFromPoints(oContourObj, MeshPointSet::PT_OCONTOUR);
    }
  }

}

bool CardiacMeshReaderUI::loadPolyDataFromPoints(rtPolyDataObject* data,  MeshPointSet::PointType type) {
  if (!data) return false;

  rtPolyDataObject::PolyPoint temp;
  rtPolyDataObject::PolyPointLink tempLink;
  QList<rtPolyDataObject::PolyPoint> pointList;
  QList<rtPolyDataObject::PolyPointLink> pointListLink;

  if (type == MeshPointSet::PT_ICONTOUR) {
    temp.color[0] = 255.0;
    temp.color[1] = 0.0;
    temp.color[2] = 0.0;
  } else if (type == MeshPointSet::PT_OCONTOUR) {
    temp.color[0] = 0.0;
    temp.color[1] = 0.0;
    temp.color[2] = 255.0;
  } else {
    temp.color[0] = 255.0;
    temp.color[1] = 255.0;
    temp.color[2] = 255.0;
  }

  pointList.clear();
  pointListLink.clear();

  double space[3];
  m_customReader.getImageData()->GetSpacing(space);

  vtkTransform* trans = m_customReader.getTransform();
  //vtkTransform* trans = vtkTransform::New();
  trans->Inverse();
  // ASSUME 20 PHASES
  int numPhases=20;
  int maxSlices=0;
  for (int ix1=0; ix1<=numPhases; ix1++) {
    MeshPointSet* currPhase=NULL;
    currPhase = m_meshReader.getPointSet(ix1);
    if(!currPhase) continue;
    if (currPhase->getMaxSlice() > maxSlices) {
      maxSlices = currPhase->getMaxSlice();
    }
  }

  for (int ix1=0; ix1<=numPhases; ix1++) {
    MeshPointSet* currPhase=NULL;
    currPhase = m_meshReader.getPointSet(ix1);
    if(!currPhase) continue;

    int numVertices=0;
    for (double curvePos=0.0; curvePos<=maxSlices; curvePos+=0.1) {
      for (int ix2=0; ix2<=currPhase->getMaxPtNum(); ix2++) {
        temp.ptList[0] = currPhase->getInterpolateXValue(type, curvePos, ix2)*space[0];
        temp.ptList[1] = currPhase->getInterpolateYValue(type, curvePos, ix2)*space[1];
        temp.ptList[2] = (curvePos*space[2]+0.5f*space[2]);

        trans->TransformPoint(temp.ptList, temp.ptList);
        pointList.append(temp);
        numVertices++;
      }
    }

    for (int ix2=0; ix2<(numVertices-202); ix2++) {
      tempLink.threeVertex[0] = ix2;
      tempLink.threeVertex[1] = ix2+1;
      tempLink.threeVertex[2] = ix2+201;
      pointListLink.append(tempLink);

      tempLink.threeVertex[0] = ix2+1;
      tempLink.threeVertex[1] = ix2+201;
      tempLink.threeVertex[2] = ix2+202;
      pointListLink.append(tempLink);
    }

    data->lock();
    data->setNewGeometry( &pointList, &pointListLink, m_customReader.getTriggerList()->at(ix1) );
    data->Modified();
    data->unlock();

    // Reset for the next round.
    pointList.clear();
    pointListLink.clear();
  }
trans->Inverse();
}
