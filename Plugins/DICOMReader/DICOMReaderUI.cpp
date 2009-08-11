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
      ptObj->copyNewImageData(m_customReader.getImageData());
      ptObj->copyNewTransform(m_customReader.getTransform());
      ptObj->Modified();
      ptObj->unlock();
    }
  }
}
