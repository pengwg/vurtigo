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
#ifndef CARTO_FILE_READER_H
#define CARTO_FILE_READER_H

#include <QMap>
#include <QList>
#include <QString>

class CartoFileReader
{
public:
  CartoFileReader();
  ~CartoFileReader();

  struct CartoPoint {
    int col1;
    int id;
    int col3;
    double x, y, z;
  };

  //! Read a particular carto file.
  bool readFile(QString fName);

  //! Get a list of all the carto points loaded by the reader.
  QList<CartoPoint> getPointSet();

  //! Get the name of the dataset.
  /*! This is usually the first line of the file. */
  QString getDataName();

protected:
  //! A map between an object ID and the object itself.
  QMap<int, CartoPoint> m_pointList;

  //! The name of the dataset which is found on the first line of the file.
  QString m_dataName;
};

#endif
