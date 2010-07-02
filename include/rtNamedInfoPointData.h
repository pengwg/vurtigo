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

#ifndef RTNAMEDINFOPOINTDATA_H
#define RTNAMEDINFOPOINTDATA_H

#include <QString>

#include "rtBasic3DPointData.h"

class rtNamedInfoPointData : public rtBasic3DPointData
{
public:
  //! Constructor
  rtNamedInfoPointData();

  //! Copy constructor
  rtNamedInfoPointData(const rtNamedInfoPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtNamedInfoPointData &other) const;
  //! Assignment operator
  rtNamedInfoPointData& operator=(const rtNamedInfoPointData& sp);

  //! Set the value along with the name.
  inline void setNamedValue(QString name, double value) {
    m_namedInfo = value;
    m_name = name;
  }

  //! Get the name given to this value
  inline QString getName() { return m_name; }
  //! Get the value
  inline double getValue() { return m_namedInfo; }


protected:

  //! Genereic information object
  double m_namedInfo;

  //! Name of the generic info object
  QString m_name;
};

#endif // RTNAMEDINFOPOINTDATA_H
