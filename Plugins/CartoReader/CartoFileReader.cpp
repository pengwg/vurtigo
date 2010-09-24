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
  rtCartoPointData pt;

  m_pointList.clear();

  int temp1, temp2;
  int pId;
  double pX,pY,pZ;
  double pAlpha,pBeta,pGamma;
  double pUniPolar, pBiPolar;
  int pLAT;
  bool firstPoint = true;


  while (!ts.atEnd()) {
    while (temp!='P' && !ts.atEnd()) ts >> temp;
    if (!ts.atEnd()) {
      ts >> temp1 >> pId >> temp2 >> pX >> pY >> pZ;
      pt.setPointId(pId);
      pt.setX(pX);
      pt.setY(pY);
      pt.setZ(pZ);

      ts >> pAlpha >> pBeta >> pGamma;
      pt.setAlpha(pAlpha);
      pt.setBeta(pBeta);
      pt.setGamma(pGamma);

      ts >> pUniPolar >> pBiPolar >> pLAT;
      pt.setUniPolar(pUniPolar);
      pt.setBiPolar(pBiPolar);
      pt.setLAT(pLAT);

      pt.setTriggerDelay(0); // No trigger delay for this type of file.
      m_pointList.insert(pt.getPointId(), pt);

      if (firstPoint) {
        m_minUniPolar = pt.getUniPolar();
        m_maxUniPolar = pt.getUniPolar();
        m_minBiPolar = pt.getBiPolar();
        m_maxBiPolar = pt.getBiPolar();
        m_minLAT = pt.getLAT();
        m_maxLAT = pt.getLAT();
        m_maxTrigDelay = pt.getTriggerDelay();
        m_minTrigDelay = pt.getTriggerDelay();
        firstPoint = false;
      } else if (pt.getUniPolar() < m_minUniPolar) {
        m_minUniPolar = pt.getUniPolar();
      } else if (pt.getUniPolar() > m_maxUniPolar) {
        m_maxUniPolar = pt.getUniPolar();
      }

      if (pt.getBiPolar() < m_minBiPolar) {
        m_minBiPolar = pt.getBiPolar();
      } else if (pt.getBiPolar() > m_maxBiPolar) {
        m_maxBiPolar = pt.getBiPolar();
      }

      if (pt.getLAT() < m_minLAT) {
        m_minLAT = pt.getLAT();
      } else if (pt.getLAT() > m_maxLAT) {
        m_maxLAT = pt.getLAT();
      }

      if (pt.getTriggerDelay() > m_maxTrigDelay) {
        m_maxTrigDelay = pt.getTriggerDelay();
      } else if ( pt.getTriggerDelay() < m_minTrigDelay) {
        m_minTrigDelay = pt.getTriggerDelay();
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
  rtCartoPointData pt;
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
        pt.setPointId( currId );
        currId++;
        pt.setX( strList.at(0).toDouble() );
        pt.setY( strList.at(1).toDouble() );
        pt.setZ( strList.at(2).toDouble() );
        pt.setBiPolar( strList.at(4).toDouble() );
        pt.setTriggerDelay( strList.at(7).toInt() );

        // Zero everything else
        pt.setAlpha( 0.0 );
        pt.setBeta( 0.0 );
        pt.setGamma( 0.0 );
        pt.setUniPolar( 0.0 );
        pt.setLAT( 0 );

        // Ensure that at least one element is non-zero.
        if ( pt.getX() != 0.0 || pt.getY() != 0.0 || pt.getZ() != 0.0 || pt.getBiPolar() != 0.0 || pt.getTriggerDelay() != 0.0 ) {
          m_pointList.insert(pt.getPointId(), pt);


          if (firstPoint) {
            m_minUniPolar = pt.getUniPolar();
            m_maxUniPolar = pt.getUniPolar();
            m_minBiPolar = pt.getBiPolar();
            m_maxBiPolar = pt.getBiPolar();
            m_minLAT = pt.getLAT();
            m_maxLAT = pt.getLAT();
            firstPoint = false;
          } else if (pt.getUniPolar() < m_minUniPolar) {
            m_minUniPolar = pt.getUniPolar();
          } else if (pt.getUniPolar() > m_maxUniPolar) {
            m_maxUniPolar = pt.getUniPolar();
          }

          if (pt.getBiPolar() < m_minBiPolar) {
            m_minBiPolar = pt.getBiPolar();
          } else if (pt.getBiPolar() > m_maxBiPolar) {
            m_maxBiPolar = pt.getBiPolar();
          }

          if (pt.getLAT() < m_minLAT) {
            m_minLAT = pt.getLAT();
          } else if (pt.getLAT() > m_maxLAT) {
            m_maxLAT = pt.getLAT();
          }

          if (pt.getTriggerDelay() > m_maxTrigDelay) {
            m_maxTrigDelay = pt.getTriggerDelay();
          } else if ( pt.getTriggerDelay() < m_minTrigDelay) {
            m_minTrigDelay = pt.getTriggerDelay();
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



QList<rtCartoPointData> CartoFileReader::getPointSet() {
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
    rtCartoPointData pt = m_pointList.value(pointKeys[ix1]);
    midCloud[0] += pt.getX();
    midCloud[1] += pt.getY();
    midCloud[2] += pt.getZ();
  }

  // Find the middle of the cloud.
  midCloud[0] = midCloud[0] / ((double)pointKeys.size());
  midCloud[1] = midCloud[1] / ((double)pointKeys.size());
  midCloud[2] = midCloud[2] / ((double)pointKeys.size());

  for (int ix1=0; ix1<pointKeys.size(); ix1++) {
    m_pointList[pointKeys[ix1]].translate(-midCloud[0], -midCloud[1], -midCloud[2]);
  }
}
