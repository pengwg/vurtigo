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

// Qt
#include <QtGlobal>

//DCMTK
#ifdef Q_OS_UNIX
#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
#endif

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"

// Local
#include "DICOMCommonData.h"


DICOMCommonData::DICOMCommonData()
 : m_warning(false), m_error(false)
{

  // Set defaults for parameters
  m_patientsName = "ANONYMOUS";
  m_studyDate = QDate::fromString("19700101", "yyyymmdd");
  m_studyTime = QTime::fromString("010101", "hhmmss");
  m_patientPosition = "HFS";
  m_modality = "";
  m_manufacturer = "";
  m_studyID = "000000";
  m_seriesNumber = "0";

  m_numRows = 0;
  m_numCols = 0;

  m_pixSpace[0] = 1.0;
  m_pixSpace[1] = 1.0;

  m_imgPosition[0] = 0.0;
  m_imgPosition[1] = 0.0;
  m_imgPosition[2] = 0.0;

  m_imgOrient[0] = 1.0;
  m_imgOrient[1] = 0.0;
  m_imgOrient[2] = 0.0;
  m_imgOrient[3] = 0.0;
  m_imgOrient[4] = 1.0;
  m_imgOrient[5] = 0.0;
}


void DICOMCommonData::readData(DcmDataset* datSet) {
  OFString temp;

  // Get the patient name.
  if (!datSet->findAndGetOFString(DCM_PatientsName, temp).good()) {
    m_warning = true;
    m_patientsName = "ANONYMOUS";
  } else {
    m_patientsName = temp.c_str();
  }

  // Get the study date.
  if (!datSet->findAndGetOFString(DCM_StudyDate, temp).good()) {
    m_warning = true;
    // yyyyMMdd
    m_studyDate = QDate::fromString("19700101", "yyyyMMdd");
  } else {
    m_studyDate = QDate::fromString(QString(temp.c_str()), "yyyyMMdd");
  }

  // Get the study time.
  if (!datSet->findAndGetOFString(DCM_StudyTime, temp).good()) {
    m_warning = true;
    // hhmmss
    m_studyTime = QTime::fromString("010101", "hhmmss");
  } else {
    m_studyTime = QTime::fromString(QString(temp.c_str()), "hhmmss");
  }

  if (!datSet->findAndGetOFString(DCM_PatientPosition, temp).good()) {
    m_warning = true;
    // Default to head-first supine.
    m_patientPosition = "HFS";
  } else {
    m_patientPosition = temp.c_str();
  }

  // Modality is required.
  if (!datSet->findAndGetOFString(DCM_Modality, temp).good()) {
    m_error = true;
  } else {
    m_modality = temp.c_str();
  }

  // Manufacturer is required.
  if(!datSet->findAndGetOFString(DCM_Manufacturer, temp).good()) {
    m_error = true;
  } else {
    m_manufacturer = temp.c_str();
  }

  // Study ID
  if (!datSet->findAndGetOFString(DCM_StudyID, temp).good()) {
    m_studyID = "000000";
    m_warning = true;
  } else {
    m_studyID = temp.c_str();
  }

  // Series Number
  if (!datSet->findAndGetOFString(DCM_SeriesNumber, temp).good()) {
    m_seriesNumber = "0";
    m_warning = true;
  } else {
    m_seriesNumber = temp.c_str();
  }

  // Number of rows is required.
  if ( !datSet->findAndGetUint16(DCM_Rows, m_numRows).good() ) {
    m_error = true;
  }

  // Number of columns is required
  if ( !datSet->findAndGetUint16(DCM_Columns, m_numCols).good() ) {
    m_error = true;
  }

  // Image pixel spacing.
  for (int ix1=0; ix1<2; ix1++) {
    if( !datSet->findAndGetFloat64(DCM_PixelSpacing, m_pixSpace[ix1], ix1).good() ) {
      m_pixSpace[ix1] = 1.0;
      m_warning = true;
    }
  }

  // Image position
  for (int ix1=0; ix1<3; ix1++) {
    if( !datSet->findAndGetFloat64(DCM_ImagePositionPatient, m_imgPosition[ix1], ix1).good() ) {
      m_error = true;
    }
  }

  // Image direction cosines.
  for (int ix1=0; ix1<6; ix1++) {
    if( !datSet->findAndGetFloat64(DCM_ImageOrientationPatient, m_imgOrient[ix1], ix1).good() ) {
      m_error = true;
    }
  }

}


void DICOMCommonData::deepCopy(DICOMCommonData* data) {
  m_warning = data->isWarning();
  m_error = data->isError();

  setPatientName(data->getPatientName());
  setStudyDate(data->getStudyDate());
  setStudyTime(data->getStudyTime());
  setPatientPosition(data->getPatientPosition());
  setModality(data->getModality());
  setManufacturer(data->getManufacturer());
  setStudyID(data->getStudyID());
  setSeriesNumber(data->getSeriesNumber());

  setNumRows(data->getNumRows());
  setNumCols(data->getNumCols());

  data->getPixelSpacing(m_pixSpace);
  data->getImagePosition(m_imgPosition);
  data->getImageOrientation(m_imgOrient);
}


void DICOMCommonData::getPixelSpacing(double pixSpace[2]) {
  pixSpace[0] = m_pixSpace[0];
  pixSpace[1] = m_pixSpace[1];
}

void DICOMCommonData::getImagePosition(double imgPosition[3]) {
  imgPosition[0] = m_imgPosition[0];
  imgPosition[1] = m_imgPosition[1];
  imgPosition[2] = m_imgPosition[2];
}

void DICOMCommonData::getImageOrientation(double imgOrient[6]) {
  imgOrient[0] = m_imgOrient[0];
  imgOrient[1] = m_imgOrient[1];
  imgOrient[2] = m_imgOrient[2];
  imgOrient[3] = m_imgOrient[3];
  imgOrient[4] = m_imgOrient[4];
  imgOrient[5] = m_imgOrient[5];
}

void DICOMCommonData::setPixelSpacing(double pixSpace[2]) {
  m_pixSpace[0] = pixSpace[0];
  m_pixSpace[1] = pixSpace[1];
}

void DICOMCommonData::setImagePosition(double imgPosition[3]) {
  m_imgPosition[0] = imgPosition[0];
  m_imgPosition[1] = imgPosition[1];
  m_imgPosition[2] = imgPosition[2];
}

void DICOMCommonData::setImageOrientation(double imgOrient[6]) {
  m_imgOrient[0] = imgOrient[0];
  m_imgOrient[1] = imgOrient[1];
  m_imgOrient[2] = imgOrient[2];
  m_imgOrient[3] = imgOrient[3];
  m_imgOrient[4] = imgOrient[4];
  m_imgOrient[5] = imgOrient[5];
}
