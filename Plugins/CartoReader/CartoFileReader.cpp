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

#include <iostream>

CartoFileReader::CartoFileReader() {
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

    }
    temp = ' ';
  }

  // Cleanup.
  f.close();

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
