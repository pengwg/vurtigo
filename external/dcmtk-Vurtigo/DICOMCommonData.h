/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef DICOMCOMMONDATA_H
#define DICOMCOMMONDATA_H

// Qt include
#include <QString>
#include <QDate>
#include <QTime>

class DcmDataset;

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

  //! Copy all of the data from the parameter into this class.
  /*!
    Copy all of the data from the parameter into this class. This includes date, time and error and warning flags.
    \param data The data object to be copied. This data object is not modified.
   */
  void deepCopy(DICOMCommonData* data);

  ////
  // Functions to GET data
  ////
  inline QString getPatientName() { return m_patientsName; }
  inline QDate getStudyDate() { return m_studyDate; }
  inline QTime getStudyTime() { return m_studyTime; }
  inline QString getPatientPosition() { return m_patientPosition; }
  inline QString getModality() { return m_modality; }
  inline QString getManufacturer() { return m_manufacturer; }
  inline QString getStudyID() { return m_studyID; }
  inline QString getSeriesNumber() { return m_seriesNumber; }

  inline unsigned short getNumRows() { return m_numRows; }
  inline unsigned short getNumCols() { return m_numCols; }

  void getPixelSpacing(double pixSpace[2]);
  void getImagePosition(double imgPosition[3]);
  void getImageOrientation(double imgOrient[6]);

  ////
  // Functions to SET data
  ////
  inline void setPatientName(QString name) { m_patientsName = name; }
  inline void setStudyDate(QDate date) { m_studyDate = date; }
  inline void setStudyTime(QTime time) { m_studyTime = time; }
  inline void setPatientPosition(QString pos) { m_patientPosition = pos; }
  inline void setModality(QString modality) { m_modality = modality; }
  inline void setManufacturer(QString manufacturer) { m_manufacturer = manufacturer; }
  inline void setStudyID(QString studyID) { m_studyID = studyID; }
  inline void setSeriesNumber(QString seriesNum) { m_seriesNumber = seriesNum; }

  inline void setNumRows(unsigned int rows) { m_numRows=rows; }
  inline void setNumCols(unsigned int cols) { m_numCols=cols; }

  void setPixelSpacing(double pixSpace[2]);
  void setImagePosition(double imgPosition[3]);
  void setImageOrientation(double imgOrient[6]);

protected:
  bool m_warning;
  bool m_error;


  QString m_patientsName;
  QDate m_studyDate;
  QTime m_studyTime;
  QString m_patientPosition;
  QString m_modality;
  QString m_manufacturer;
  QString m_studyID;
  QString m_seriesNumber;

  //! Number of rows in the rows
  unsigned short m_numRows;

  //! Number of columns
  unsigned short m_numCols;

  //! Pixel spacing in the x ad y directions
  double m_pixSpace[2];

  //! Position in 3D of the image.
  double m_imgPosition[3];

  //! Direction cosines for the x and y directions.
  double m_imgOrient[6];
};

#endif // DICOMCOMMONDATA_H
