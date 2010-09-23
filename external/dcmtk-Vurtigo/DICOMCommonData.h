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

#ifndef DICOMCOMMONDATA_H
#define DICOMCOMMONDATA_H

// Qt include
#include <QString>
#include <QDate>
#include <QTime>

#ifdef Q_OS_UNIX
#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
#endif

// DCMTK include
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcdatset.h"

//! Class to maintain the common elements that all DICOM files should contain.
/*!
  This class is responsable for reading a set of DICOM tags that should be contained in ALL standard DICOM files. If some of these tag are missing they can be replaced by defaults. If a tag is replaced by a default then a warning is given. However, some tags may not be replaced by defaults and will be flagged as an error if they are missing.
  */
class DICOMCommonData
{
public:
  DICOMCommonData();

  inline bool isWarning() { return m_warning; }
  inline bool isError() { return m_error; }
  inline void resetFlags() { m_warning = false; m_error=false; }

  //! Try to read the data.
  /*!
    If the reading of some of the tags fails then the warning or error flags are set.
    */
  void readData(DcmDataset* datSet);


  QString getPatientName();
  QDate getStudyDate();
  QTime getStudyTime();
  QString getPatientPosition();
  QString getModality();
  QString getManufacturer();

protected:
  bool m_warning;
  bool m_error;


  OFString m_patientsName;
  OFString m_studyDate;
  OFString m_studyTime;
  OFString m_patientPosition;
  OFString m_modality;
  OFString m_manufacturer;

};

#endif // DICOMCOMMONDATA_H
