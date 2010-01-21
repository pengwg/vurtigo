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
#include "rtBaseHandle.h"
#include "DICOMFileReader.h"

CardiacMeshReaderUI::CardiacMeshReaderUI() {
  setupUi(this);

  m_lastDir = "";

  connectSignals();
}

CardiacMeshReaderUI::~CardiacMeshReaderUI() {
}

void CardiacMeshReaderUI::connectSignals() {
  connect(directoryEdit, SIGNAL(editingFinished()), this, SLOT(newDirectory()));
  connect(directoryChooser, SIGNAL(clicked()), this, SLOT(dirChooser()));
  connect(createVolumeButton, SIGNAL(clicked()), this, SLOT(saveAsVolume()));
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
      createVolumeButton->setEnabled(true);
      nameLineEdit->setEnabled(true);
    } else {
      infoBrowser->append("Error!");
      createVolumeButton->setEnabled(false);
      nameLineEdit->setEnabled(false);
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

//! Save the current set of DICOM images as a volume
void CardiacMeshReaderUI::saveAsVolume() {
  m_vol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, nameLineEdit->text());
  if (m_vol >=0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

    if (ptObj) {
      ptObj->lock();
      ptObj->copyNewTransform(m_customReader.getTransform());
      ptObj->copyNewImageData(m_customReader.getImageData());
      ptObj->Modified();
      ptObj->unlock();
    }
  }
}
