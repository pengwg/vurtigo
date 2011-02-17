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
#include "DICOMReaderUI.h"

#include <QFileDialog>

#include <iostream>

#include <vtkErrorCode.h>

#include "rt3dVolumeDataObject.h"
#include "rtBaseHandle.h"
#include "DICOMFileReader.h"

DICOMReaderUI::DICOMReaderUI() {
  setupUi(this);

  m_lastDir = "";

  connectSignals();
}

DICOMReaderUI::~DICOMReaderUI() {
}

void DICOMReaderUI::connectSignals() {
  connect(directoryEdit, SIGNAL(editingFinished()), this, SLOT(newDirectory()));
  connect(directoryChooser, SIGNAL(clicked()), this, SLOT(dirChooser()));
  connect(createVolumeButton, SIGNAL(clicked()), this, SLOT(saveAsVolume()));
}

//! Slot called when the user changes the directory.
void DICOMReaderUI::newDirectory() {

  if ( m_lastDir != directoryEdit->text() ) {
    m_lastDir = directoryEdit->text();

    // Use the custom reader too.
    bool okDir = m_customReader.setDirectory(m_lastDir);
    bool okVol = false;
    if (okDir) {
        // Try to create the volume.
        okVol = m_customReader.createVolume(m_customReader.getDICOMImageData());
    }
    infoBrowser->clear();
    if (okDir && okVol) {
      infoBrowser->append(m_customReader.getComments());

      createVolumeButton->setEnabled(true);
      nameLineEdit->setText(m_customReader.getDefaultName());
      nameLineEdit->setEnabled(true);
    } else {
      createVolumeButton->setEnabled(false);
      nameLineEdit->setEnabled(false);
      if (!okDir)
      {
          infoBrowser->append("Error with directory!");
      }
      if (!okVol)
      {
          infoBrowser->append("Could not create volume!");
      }
    }

  }
}

//! Popup a dialog box to choose a directory
void DICOMReaderUI::dirChooser() {
  QString dir;

  dir = QFileDialog::getExistingDirectory(this, "Select DICOM Directory", "", QFileDialog::ShowDirsOnly);

  if (dir != "") {
    // The user did select a directory.
    directoryEdit->setText(dir);
    newDirectory();
  }
}

//! Save the current set of DICOM images as a volume
void DICOMReaderUI::saveAsVolume() {
  m_vol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, nameLineEdit->text());
  if (m_vol >=0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

    if (ptObj) {
      ptObj->lock();
      ptObj->copyNewTransform(m_customReader.getTransform());
      ptObj->copyNewImageData(m_customReader.getImageData());
      ptObj->copyTriggerDelayList(m_customReader.getTriggerList());
      ptObj->copyDicomCommonData(m_customReader.getCommonDataHandle());
      ptObj->Modified();
      ptObj->unlock();
    }
  }
}
