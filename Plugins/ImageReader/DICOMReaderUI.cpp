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
#include "rtApplication.h"
#include "rtObjectManager.h"

DICOMReaderUI::DICOMReaderUI() {
  setupUi(this);

  m_lastDir = "";
  isDICOM = true;

  connectSignals();
}

DICOMReaderUI::~DICOMReaderUI() {
}

void DICOMReaderUI::connectSignals() {
  connect(directoryEdit, SIGNAL(editingFinished()), this, SLOT(newDirectory()));
  connect(directoryChooser, SIGNAL(clicked()), this, SLOT(dirChooser()));
  connect(createVolumeButton, SIGNAL(clicked()), this, SLOT(saveAsVolume()));
  connect(spacingX, SIGNAL(valueChanged(double)), this, SLOT(spacingChanged()));
  connect(spacingY, SIGNAL(valueChanged(double)), this, SLOT(spacingChanged()));
  connect(spacingZ, SIGNAL(valueChanged(double)), this, SLOT(spacingChanged()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(setupVolumes()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(setupVolumes()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectRenamed(int)), this, SLOT(setupVolumes()));
}

//! Slot called when the user changes the directory.
void DICOMReaderUI::newDirectory() {

  if ( m_lastDir != directoryEdit->text() ) {
    m_lastDir = directoryEdit->text();

    DICOMFileReader::imageType type;
    if (imageTypeBox->currentIndex() == 0)
    {
        type = DICOMFileReader::I_DICOM;
        isDICOM = true;
    }
    else
    {
        isDICOM = false;
        if (imageTypeBox->currentIndex() == 1)
            type = DICOMFileReader::I_JPEG;
        else if (imageTypeBox->currentIndex() == 2)
            type = DICOMFileReader::I_PNG;
        else if (imageTypeBox->currentIndex() == 3)
            type = DICOMFileReader::I_BMP;
    }

    // Use the custom reader too.
    bool okDir = m_customReader.setDirectory(m_lastDir,type);
    bool okVol = true;
    if (okDir && type == DICOMFileReader::I_DICOM) {
        // Try to create the volume.
        okVol = m_customReader.createVolume(m_customReader.getDICOMImageData());
    }

    infoBrowser->clear();
    if (okDir && okVol) {
      infoBrowser->append(m_customReader.getComments());
      if (type != DICOMFileReader::I_DICOM)
          spacingOptions->setEnabled(true);
      createVolumeButton->setEnabled(true);
      nameLineEdit->setText(m_customReader.getDefaultName());
      nameLineEdit->setEnabled(true);
    } else {
      spacingOptions->setEnabled(false);
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
  QDir lastDirUp = QDir(m_lastDir);
  lastDirUp.cdUp();
  dir = QFileDialog::getExistingDirectory(this, "Select Image Directory", lastDirUp.path(), QFileDialog::ShowDirsOnly);

  if (dir != "") {
    // The user did select a directory.
    directoryEdit->setText(dir);
    newDirectory();
  }
}

//! Save the current set of DICOM images as a volume
void DICOMReaderUI::saveAsVolume() {

    if (isDICOM)
    {
        m_vol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, nameLineEdit->text());
        if (m_vol >=0) {
            rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

            if (ptObj) {
                ptObj->lock();
                ptObj->copyNewTransform(m_customReader.getTransform());
                ptObj->copyNewImageData(m_customReader.getImageData(isDICOM));
                ptObj->copyTriggerDelayList(m_customReader.getTriggerList());
                ptObj->copyDicomCommonData(m_customReader.getCommonDataHandle());
                ptObj->Modified();
                ptObj->unlock();
            }
        }
    }
    else
    {
        QList<int> vols = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_3DObject);
        rt3DVolumeDataObject *v = NULL;
        if (volumeBox->currentIndex() >= 0)
            v = static_cast<rt3DVolumeDataObject *>(rtBaseHandle::instance().getObjectWithID(vols.at(volumeBox->currentIndex())));
        m_vol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, nameLineEdit->text());
        if (m_vol >=0) {
            rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_vol));

            if (ptObj) {
                ptObj->lock();
                if (autoRadio->isChecked() && v)
                {
                    double s[3];
                    v->getSpacing(s);
                    m_customReader.setSpacing(s[0],s[1],s[2]);
                    ptObj->copyNewTransform(v->getTransform());
                }
                ptObj->copyNewImageData(m_customReader.getImageData(isDICOM),1);
                ptObj->Modified();
                ptObj->unlock();
            }
        }
    }
}

void DICOMReaderUI::setupVolumes()
{
    volumeBox->clear();
    QList<int> vols = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_3DObject);
    rt3DVolumeDataObject *v;
    for (int ix1=0; ix1<vols.size(); ix1++)
    {
        v = static_cast<rt3DVolumeDataObject *>(rtBaseHandle::instance().getObjectWithID(vols[ix1]));
        volumeBox->addItem(v->getObjName() + " " + QString::number(vols[ix1]));
    }
}
