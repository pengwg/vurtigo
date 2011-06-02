#ifndef DICOM_IMAGE_DATA_H
#define DICOM_IMAGE_DATA_H

// Qt include
#include <QString>
#include <QDate>
#include <QTime>
#include <QList>

// DCMTK
#ifdef Q_OS_UNIX
#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
#endif

#include "dcmtk/ofstd/oftypes.h"


// Local
#include "DICOMCommonData.h"


//! The relevant image data that can be read from a DICOM file. 
class DICOMImageData {
public:
  DICOMImageData();
  ~DICOMImageData();

  void setTrigList(QList<double>* list) { m_trigList=list; }

  //! Read a dicom file.
  bool readFile(QString fName);

  //! Get the trigger delay from a particular file. 
  static double getTriggerFromFile(QString fName);

  QString getPatientName();
  QDate getStudyDate();
  QTime getStudyTime();
  QString getPatientPosition();
  QString getModality();
  QString getManufacturer();
  inline QString getStudyID() { return m_commonData.getStudyID(); }
  inline QString getSeriesNumber() { return m_commonData.getSeriesNumber(); }

  //! Get the number of rows in one image
  inline unsigned short getNumRows() { return m_commonData.getNumRows(); }

  //! Get the number of columns in one image
  inline unsigned short getNumCols() { return m_commonData.getNumCols(); }

  //! Get the pixel spacing for x or y.
  /*!
  \param p Use 0(zero) to get the x and 1(one) to get the y. 
  */
  double getPixelSpace(int p);

  //! Get the image position as x, y, or z.
  /*!
  \param p Use 0(zero) to get the x, 1(one) to get the y and 2(two) to get the z. 
  */
  double getImagePosition(int p);

  //! Get the image orientation from one of the direction cosines.
  double getImageOrientation(int cosine, int p);

  //! Check if the data that was read is cine.
  bool isCineData();
  unsigned short getImagesPerCycle() { return m_imagesPerCycle; }

  inline short* getDataPtr() {return m_shortData;}

  inline double getTriggerTime() { return m_triggerTime; }

  inline DICOMCommonData* getCommonDataHandle() { return &m_commonData; }

protected:
  bool readGE_MR(DcmDataset* datSet);
  bool readPhilips_MR(DcmDataset* datSet);
  bool readPhilips_CT(DcmDataset* datSet);
  bool readHeartVista_MR(DcmDataset* datSet);

  QList<double> *m_trigList;

  //! Common data to all DICOM headers.
  DICOMCommonData m_commonData;


  unsigned short m_imagesPerCycle; // Number of images in one cardiac cycle.
  unsigned short m_cardiacPhase; // The cardiac phase of the current image.
  unsigned short m_sine;
  Uint32 m_pixelGroupLen;
  unsigned long m_numElements;
  double m_triggerTime;
  short* m_shortData; // The data for the image
};


#endif
