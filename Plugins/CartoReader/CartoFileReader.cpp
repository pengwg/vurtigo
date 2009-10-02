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
  m_pointList.clear();
}


CartoFileReader::~CartoFileReader() {

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
  CartoPoint pt;

  m_pointList.clear();

  while (!ts.atEnd()) {
    while (temp!='P' && !ts.atEnd()) ts >> temp;
    if (!ts.atEnd()) {
      ts >> pt.col1 >> pt.id >> pt.col3 >> pt.x >> pt.y >> pt.z;
      m_pointList.insert(pt.id, pt);
    }
    temp = ' ';
  }

  // Cleanup.
  f.close();

  return true;
}


QList<CartoFileReader::CartoPoint> CartoFileReader::getPointSet() {
  return m_pointList.values();
}


QString CartoFileReader::getDataName() {
  return m_dataName;
}
