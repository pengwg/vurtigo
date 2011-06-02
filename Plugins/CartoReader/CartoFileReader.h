#ifndef CARTO_FILE_READER_H
#define CARTO_FILE_READER_H

#include <QMap>
#include <QList>
#include <QString>

#include <vtkTransform.h>

#include "rtCartoPointData.h"

class CartoFileReader
{
public:
  CartoFileReader();
  ~CartoFileReader();

  //! Read a particular carto file.
  /*! Read the file with the name fName. Will return true if the file was read as expected and false otherwise. Note that this function will still return true if an empty carto file was read. As a result it is important to check the point list to verify that there exists at least one point in the list.
    \param fName The full name of the file to read. It is assumed that the string contains not just the name of the file put also the full path.
    \return Will return true if the function was able to read the file as expected and false otherwise.
      */
  bool readFile(QString fName);

  //! Read an Xml Version of the carto file.
  bool readXmlFile(QString fName);

  //! Get a list of all the carto points loaded by the reader.
  QList<rtCartoPointData> getPointSet();

  //! Get the point with a specific ID.
  inline rtCartoPointData getPointWithId(int id) { return m_pointList.value(id); }

  //! Get the name of the dataset.
  /*! The name of the dataset is taken from the first line of the file. Carto files have one line of text at the top that does not have any point informtaion. */
  QString getDataName();

  double getMinUniPolar() { return m_minUniPolar; }
  double getMaxUniPolar() { return m_maxUniPolar; }
  double getMinBiPolar() { return m_minBiPolar; }
  double getMaxBiPolar() { return m_maxBiPolar; }
  int getMinLAT() { return m_minLAT; }
  int getMaxLAT() { return m_maxLAT; }
  unsigned int getMinTrigDelay() { return m_minTrigDelay; }
  unsigned int getMaxTrigDelay() { return m_maxTrigDelay; }

protected:
  //! A map between an object ID and the object itself.
  QMap<int, rtCartoPointData> m_pointList;

  //! The name of the dataset which is found on the first line of the file.
  QString m_dataName;

  //! Minimum unipolar
  double m_minUniPolar;
  //! Maximum unipolar
  double m_maxUniPolar;

  //! Min bipolar
  double m_minBiPolar;
  //! Max bipolar
  double m_maxBiPolar;

  //! Min LAT
  int m_minLAT;
  //! Max LAT
  int m_maxLAT;

  //! The minimum value for the trigger delay
  unsigned int m_minTrigDelay;

  //! The maximum value for the trigger delay
  unsigned int m_maxTrigDelay;

  //! Flag to determine if a valid flag has been loaded.
  bool m_fileLoaded;

  //! Center the points around the origin.
  void centerPoints();
};

#endif
