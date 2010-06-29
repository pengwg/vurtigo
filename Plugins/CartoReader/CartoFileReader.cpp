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
#include "CartoFileReader.h"

#include <QDir>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>

#include <iostream>

CartoFileReader::CartoFileReader()
 : m_minTrigDelay(0), m_maxTrigDelay(0)
{
  m_fileLoaded = false;
  m_dataName = "";
  m_pointList.clear();
}


CartoFileReader::~CartoFileReader() {
  m_fileLoaded = false;
}

bool CartoFileReader::readFile(QString fName) {
  if (fName.size() < 1) return false;

  QFile f(fName);

  if (!f.exists()) return false;
  if (!f.open(QIODevice::ReadOnly)) return false;

  // At this point the file exists and is open in read-only mode.
  QTextStream ts(&f);
  char temp=' ';
  m_dataName=ts.readLine(256);
  m_dataName = m_dataName.simplified();
  m_dataName.remove(" ");
  CartoPoint pt;

  m_pointList.clear();

  int temp1, temp2;
  bool firstPoint = true;

  while (!ts.atEnd()) {
    while (temp!='P' && !ts.atEnd()) ts >> temp;
    if (!ts.atEnd()) {
      ts >> temp1 >> pt.id >> temp2 >> pt.x >> pt.y >> pt.z;
      ts >> pt.alpha >> pt.beta >> pt.gamma;
      ts >> pt.uniPolar >> pt.biPolar >> pt.LAT;
      pt.triggerDelay = 0; // No trigger delay for this type of file.
      m_pointList.insert(pt.id, pt);

      if (firstPoint) {
        m_minUniPolar = pt.uniPolar;
        m_maxUniPolar = pt.uniPolar;
        m_minBiPolar = pt.biPolar;
        m_maxBiPolar = pt.biPolar;
        m_minLAT = pt.LAT;
        m_maxLAT = pt.LAT;
        m_maxTrigDelay = pt.triggerDelay;
        m_minTrigDelay = pt.triggerDelay;
        firstPoint = false;
      } else if (pt.uniPolar < m_minUniPolar) {
        m_minUniPolar = pt.uniPolar;
      } else if (pt.uniPolar > m_maxUniPolar) {
        m_maxUniPolar = pt.uniPolar;
      }

      if (pt.biPolar < m_minBiPolar) {
        m_minBiPolar = pt.biPolar;
      } else if (pt.biPolar > m_maxBiPolar) {
        m_maxBiPolar = pt.biPolar;
      }

      if (pt.LAT < m_minLAT) {
        m_minLAT = pt.LAT;
      } else if (pt.LAT > m_maxLAT) {
        m_maxLAT = pt.LAT;
      }

      if (pt.triggerDelay > m_maxTrigDelay) {
        m_maxTrigDelay = pt.triggerDelay;
      } else if ( pt.triggerDelay < m_minTrigDelay) {
        m_minTrigDelay = pt.triggerDelay;
      }

    }
    temp = ' ';
  }

  // Cleanup.
  f.close();

  // Center the points around the origin.
  centerPoints();

  // The file is now loaded.
  m_fileLoaded=true;

  return true;
}

bool CartoFileReader::readXmlFile(QString fName) {
  if (fName.size() < 1) return false;
  bool inMapData=false;
  QFile f(fName);
  QFileInfo fInfo(fName);
  int currId;

  if (!f.exists()) return false;
  if (!f.open(QIODevice::ReadOnly)) return false;

  m_dataName=fInfo.fileName();
  CartoPoint pt;
  m_pointList.clear();

  QXmlStreamReader xml(&f);
  QStringList strList;
  currId = 1;
  bool firstPoint = true;

  while (!xml.atEnd()) {
    xml.readNext();

    if ( xml.tokenType() == QXmlStreamReader::StartElement) {
      if ( xml.name() == "map_data" ) {
        inMapData = true;
      } else if (xml.name() == "item" && inMapData) {
        strList = xml.readElementText().split(" ");
        pt.id = currId;
        currId++;
        pt.x = strList.at(0).toDouble();
        pt.y = strList.at(1).toDouble();
        pt.z = strList.at(2).toDouble();
        pt.biPolar = strList.at(4).toDouble();
        pt.triggerDelay = strList.at(7).toInt();

        // Zero everything else
        pt.alpha = 0.0;
        pt.beta = 0.0;
        pt.gamma = 0.0;
        pt.uniPolar = 0.0;
        pt.LAT = 0.0;

        // Ensure that at least one element is non-zero.
        if (pt.x != 0.0 || pt.y != 0.0 || pt.z != 0.0 || pt.biPolar != 0.0 || pt.triggerDelay!=0.0 ) {
          m_pointList.insert(pt.id, pt);


          if (firstPoint) {
            m_minUniPolar = pt.uniPolar;
            m_maxUniPolar = pt.uniPolar;
            m_minBiPolar = pt.biPolar;
            m_maxBiPolar = pt.biPolar;
            m_minLAT = pt.LAT;
            m_maxLAT = pt.LAT;
            firstPoint = false;
          } else if (pt.uniPolar < m_minUniPolar) {
            m_minUniPolar = pt.uniPolar;
          } else if (pt.uniPolar > m_maxUniPolar) {
            m_maxUniPolar = pt.uniPolar;
          }

          if (pt.biPolar < m_minBiPolar) {
            m_minBiPolar = pt.biPolar;
          } else if (pt.biPolar > m_maxBiPolar) {
            m_maxBiPolar = pt.biPolar;
          }

          if (pt.LAT < m_minLAT) {
            m_minLAT = pt.LAT;
          } else if (pt.LAT > m_maxLAT) {
            m_maxLAT = pt.LAT;
          }

          if (pt.triggerDelay > m_maxTrigDelay) {
            m_maxTrigDelay = pt.triggerDelay;
          } else if ( pt.triggerDelay < m_minTrigDelay) {
            m_minTrigDelay = pt.triggerDelay;
          }
        }
      }
    }

    if ( xml.tokenType() == QXmlStreamReader::EndElement) {
      if ( xml.name() == "map_data" ) {
        inMapData = false;
      }
    }
  }
  if (xml.hasError()) {

  }
  // Cleanup.
  f.close();

  // Center the points around the origin.
  centerPoints();

  // The file is now loaded.
  m_fileLoaded=true;

  return true;
}



QList<CartoFileReader::CartoPoint> CartoFileReader::getPointSet() {
  return m_pointList.values();
}


QString CartoFileReader::getDataName() {
  return m_dataName;
}

void CartoFileReader::centerPoints() {
  double midCloud[3];
  QList<int> pointKeys = m_pointList.keys();

  midCloud[0] = 0.0;
  midCloud[1] = 0.0;
  midCloud[2] = 0.0;

  // Add all the point values.
  for (int ix1=0; ix1<pointKeys.size(); ix1++) {
    CartoPoint pt = m_pointList.value(pointKeys[ix1]);
    midCloud[0] += pt.x;
    midCloud[1] += pt.y;
    midCloud[2] += pt.z;
  }

  // Find the middle of the cloud.
  midCloud[0] = midCloud[0] / ((double)pointKeys.size());
  midCloud[1] = midCloud[1] / ((double)pointKeys.size());
  midCloud[2] = midCloud[2] / ((double)pointKeys.size());

  for (int ix1=0; ix1<pointKeys.size(); ix1++) {
    m_pointList[pointKeys[ix1]].x -= midCloud[0];
    m_pointList[pointKeys[ix1]].y -= midCloud[1];
    m_pointList[pointKeys[ix1]].z -= midCloud[2];
  }
}
