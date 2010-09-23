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

// Local
#include "DICOMCommonData.h"

//DCMTK
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"


DICOMCommonData::DICOMCommonData()
 : m_warning(false), m_error(false)
{
}


void DICOMCommonData::readData(DcmDataset* datSet) {

  // Get the patient name.
  if (!datSet->findAndGetOFString(DCM_PatientsName, m_patientsName).good()) {
    m_warning = true;
    m_patientsName = "ANONYMOUS";
  }

  // Get the study date.
  if (!datSet->findAndGetOFString(DCM_StudyDate, m_studyDate).good()) {
    m_warning = true;
    // yyyymmdd
    m_studyDate = "19700101";
  }

  // Get the study time.
  if (!datSet->findAndGetOFString(DCM_StudyTime, m_studyTime).good()) {
    m_warning = true;
    // hhmmss
    m_studyTime = "010101";
  }

  if (!datSet->findAndGetOFString(DCM_PatientPosition, m_patientPosition).good()) {
    m_warning = true;
    // Default to head-first supine.
    m_patientPosition = "HFS";
  }

  // Modality is required.
  if (!datSet->findAndGetOFString(DCM_Modality, m_modality).good()) {
    m_error = true;
  }

  // Manufacturer is required.
  if(!datSet->findAndGetOFString(DCM_Manufacturer, m_manufacturer).good()) {
    m_error = true;
  }

}


QString DICOMCommonData::getPatientName() {
  return QString(m_patientsName.c_str());
}

QDate DICOMCommonData::getStudyDate() {
  return QDate::fromString(QString(m_studyDate.c_str()), "yyyymmdd");
}

QTime DICOMCommonData::getStudyTime() {
  return QTime::fromString(QString(m_studyTime.c_str()), "hhmmss");
}

QString DICOMCommonData::getPatientPosition() {
  return QString(m_patientPosition.c_str());
}

QString DICOMCommonData::getModality() {
  return QString(m_modality.c_str());
}

QString DICOMCommonData::getManufacturer() {
  return QString(m_manufacturer.c_str());
}
