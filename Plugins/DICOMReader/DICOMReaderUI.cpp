#include "DICOMReaderUI.h"

#include <QFileDialog>

#include <iostream>

#include <vtkErrorCode.h>

#include "rt3dVolumeDataObject.h"
#include "rtBaseHandle.h"

DICOMReaderUI::DICOMReaderUI() {
  setupUi(this);

  m_lastDir = "";
  m_imageReader = vtkDICOMImageReader::New();

  connectSignals();
}

DICOMReaderUI::~DICOMReaderUI() {
  m_imageReader->Delete();
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

    m_imageReader->SetDirectoryName(m_lastDir.toStdString().c_str());
    m_imageReader->Update();

    infoBrowser->clear();

    if (m_imageReader->GetErrorCode() == vtkErrorCode::NoError) {
      infoBrowser->append("Files open OK!");
      infoBrowser->append( QString("Study ID: ") + m_imageReader->GetStudyID() );

      float *pos;
      float* dirCos;
      pos = m_imageReader->GetImagePositionPatient();
      dirCos = m_imageReader->GetImageOrientationPatient();
      infoBrowser->append( QString("Patient Position:   ")+QString::number(pos[0]) + QString("  ") + QString::number(pos[1]) + QString("  ") + QString::number(pos[2]));
      infoBrowser->append( QString("Orientation 1: ") +QString::number(dirCos[0]) + QString("  ") + QString::number(dirCos[1]) + QString("  ") + QString::number(dirCos[2]));
      infoBrowser->append( QString("Orientation 2: ") +QString::number(dirCos[3]) + QString("  ") + QString::number(dirCos[4]) + QString("  ") + QString::number(dirCos[5]));

      createVolumeButton->setEnabled(true);
      nameLineEdit->setEnabled(true);

    } else if (m_imageReader->GetErrorCode() == vtkErrorCode::CannotOpenFileError) {
      infoBrowser->append("Cannot Open File Error!");
      createVolumeButton->setEnabled(false);
      nameLineEdit->setEnabled(false);
    } else {
      infoBrowser->append("Unknown Error!");
      createVolumeButton->setEnabled(false);
      nameLineEdit->setEnabled(false);
    }

    //m_imageReader->Print(std::cout);
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

    //m_imageReader->GetOutput()->Print(std::cout);

    if (ptObj) {
      float *pos;
      float* dirCos;
      pos = m_imageReader->GetImagePositionPatient();
      dirCos = m_imageReader->GetImageOrientationPatient();

      ptObj->lock();
      ptObj->copyNewImageData(m_imageReader->GetOutput());
      ptObj->setDirectionCosinesXY(dirCos);
      ptObj->translateData(pos[0], pos[1], pos[2]);
      ptObj->Modified();
      ptObj->unlock();
    }
  }
}
