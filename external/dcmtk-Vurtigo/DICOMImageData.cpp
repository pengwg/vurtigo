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

#include "DICOMImageData.h"

// Qt
#include <QString>

//DCMTK
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"

DICOMImageData::DICOMImageData() {
  m_shortData = NULL;
  m_trigList = NULL;
}

DICOMImageData::~DICOMImageData() {
  if (m_shortData) delete[] m_shortData;
}

bool DICOMImageData::readFile(QString fName) {
  DcmFileFormat dcmFile;
  OFCondition status;
  DcmDataset* datSet;
  bool result=true;

  // Load the DICOM file.
  status = dcmFile.loadFile(fName.toStdString().c_str());

  // Check that the file was loaded
  if (status.bad()) {
    std::cout << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
    return false;
  }
  dcmFile.loadAllDataIntoMemory();
  datSet = dcmFile.getDataset();

  m_commonData.readData(datSet);

  // Check for warnings...
  if (m_commonData.isWarning()) {
    std::cout << "Warning: Some common DICOM data was missing. Default used." << std::endl;
  }

  // Check for errors...
  if (m_commonData.isError()) {
    std::cout << "Error: Some common DICOM data was missing. No defaults available!" << std::endl;
    return false;
  }

  result = datSet->findAndGetUint16(DCM_Rows, m_numRows).good() && result;
  result = datSet->findAndGetUint16(DCM_Columns, m_numCols).good() && result;

  int ix1;
  for (ix1=0; ix1<2; ix1++) datSet->findAndGetFloat64(DCM_PixelSpacing, m_pixSpace[ix1], ix1);
  for (ix1=0; ix1<3; ix1++) datSet->findAndGetFloat64(DCM_ImagePositionPatient, m_imgPosition[ix1], ix1);
  for (ix1=0; ix1<6; ix1++) datSet->findAndGetFloat64(DCM_ImageOrientationPatient, m_imgOrient[ix1], ix1);  

  // Check that all of the standard info was read.
  if (result) {
    if ( m_commonData.getModality() == QString("MR") && m_commonData.getManufacturer() == QString("GE MEDICAL SYSTEMS") ) {
      // GE MRI
      result = readGE_MR(datSet) && result;
    } else if ( m_commonData.getModality() == QString("MR") && m_commonData.getManufacturer() == QString("Philips Medical Systems")) {
      // Philips MR
      result = readPhilips_MR(datSet) && result;
    } else if ( m_commonData.getModality() == QString("MR") && m_commonData.getManufacturer() == QString("HeartVista, Inc.")) {
      // HeartVista, Inc.
      std::cout << "Warning: HeartVista DICOM file. This type of file does not contain a complete tag set. " << std::endl;
      result = readHeartVista_MR(datSet) && result;
    } else if  ( m_commonData.getModality() == QString("CT") && m_commonData.getManufacturer() == QString("Philips") ) {
      result = readPhilips_CT(datSet) && result;
    } else {
      std::cout << "Not able to read DICOM files of type: " << m_commonData.getModality().toStdString() <<  " from " << m_commonData.getManufacturer().toStdString() << std::endl;
      result = false;
    }
  }

  return result;
}

double DICOMImageData::getTriggerFromFile(QString fName) {
  double trig;

  DcmFileFormat dcmFile;
  OFCondition status;
  DcmDataset* datSet;

  // Load the DICOM file.
  status = dcmFile.loadFile(fName.toStdString().c_str());

  // Check that the file was loaded
  if (status.bad()) {
    std::cout << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
    return false;
  }

  datSet = dcmFile.getDataset();
  if ( !datSet->findAndGetFloat64(DCM_TriggerTime, trig).good() ) {
    trig  = -1;
  }
  return trig;
}

QString DICOMImageData::getPatientName() {
  return m_commonData.getPatientName();
}

QDate DICOMImageData::getStudyDate() {
  return m_commonData.getStudyDate();
}

QTime DICOMImageData::getStudyTime() {
  return m_commonData.getStudyTime();
}

QString DICOMImageData::getPatientPosition() {
  return m_commonData.getPatientPosition();
}

QString DICOMImageData::getModality() {
  return m_commonData.getModality();
}

QString DICOMImageData::getManufacturer() {
  return m_commonData.getManufacturer();
}

double DICOMImageData::getPixelSpace(int p) { 
  if (p == 0 || p == 1) return m_pixSpace[p]; 
  return 1.0f;
}

double DICOMImageData::getImagePosition(int p) {
  if (p >= 0 && p <= 2) return m_imgPosition[p]; 
  return 0.0f;
}

double DICOMImageData::getImageOrientation(int cosine, int p) {
  if ( (cosine==0 || cosine==1) && (p >= 0 && p <= 2) ) return m_imgOrient[cosine*3+p];
  return 1.0;
}


bool DICOMImageData::isCineData() {
  if(m_imagesPerCycle>1) return true;
  return false;
}

//////////////////////
//
// Protected Functions
//
//////////////////////

bool DICOMImageData::readGE_MR(DcmDataset* datSet) {
  bool result = true;
  const unsigned short* temp;
  OFString numImgs;
  OFString currImg;

  // Check for cine.
  if (datSet->findAndGetOFString(DCM_CardiacNumberOfImages, numImgs).good()) {
    m_imagesPerCycle = QString(numImgs.c_str()).toUShort();
  }
  if (m_imagesPerCycle < 1) {
     m_imagesPerCycle = 1;
  }


  // Special GE tag for Phase. 
  if (datSet->findAndGetOFString(DcmTagKey(0x0019, 0x10d7), currImg).good()) {
    m_cardiacPhase = QString(currImg.c_str()).toUShort();
  }
  if (m_cardiacPhase < 1) {
     m_cardiacPhase = 1;
  }

  // Check for a trigger time parameter.
  if ( !datSet->findAndGetFloat64(DCM_TriggerTime, m_triggerTime).good() ) {
    m_triggerTime = 0;
  }

  datSet->findAndGetUint16(DCM_PixelRepresentation, m_sine).good();
  datSet->findAndGetUint32(DCM_PixelDataGroupLength, m_pixelGroupLen).good();
  datSet->findAndGetUint16Array(DCM_PixelData, temp, &m_numElements, false).good();

  if (m_numElements != (unsigned long)(m_numRows*m_numCols) && m_numElements > 0) {
    std::cout << "Error: Problem reading all of the dicom pixel data." << std::endl;
    return false;
  }

  if(m_shortData) delete [] m_shortData;
  m_shortData = new short[m_numElements];
  memcpy(m_shortData, temp, sizeof(short)*m_numElements);

  return result;
}

bool DICOMImageData::readPhilips_MR(DcmDataset* datSet) {
  bool result = true;
  const unsigned short* temp;
  OFString currImg;

  if (m_trigList) {
    m_imagesPerCycle = m_trigList->count();
  }

  // Check for a trigger time parameter.
  if ( !datSet->findAndGetFloat64(DCM_TriggerTime, m_triggerTime).good() ) {
    m_triggerTime = 0;
  }

  // Strange Way to Calculate Phase for Philips 
  if (datSet->findAndGetOFString(DCM_InstanceNumber, currImg).good()) {
    m_cardiacPhase = QString(currImg.c_str()).toUShort() % m_imagesPerCycle;
  }
  if (m_cardiacPhase < 1) {
     m_cardiacPhase = 1;
  }

  datSet->findAndGetUint16(DCM_PixelRepresentation, m_sine).good();
  datSet->findAndGetUint32(DCM_PixelDataGroupLength, m_pixelGroupLen).good();
  datSet->findAndGetUint16Array(DCM_PixelData, temp, &m_numElements, false).good();

  if (m_numElements != (unsigned long)(m_numRows*m_numCols) && m_numElements > 0) {
    std::cout << "Error: Problem reading all of the dicom pixel data." << std::endl;
    return false;
  }

  if(m_shortData) delete [] m_shortData;
  m_shortData = new short[m_numElements];
  memcpy(m_shortData, temp, sizeof(short)*m_numElements);

  return result;
}

bool DICOMImageData::readPhilips_CT(DcmDataset* datSet) {
  return false;
}


bool DICOMImageData::readHeartVista_MR(DcmDataset* datSet) {
  return false;
}
