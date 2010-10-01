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
#include <QList>
#include <QMap>

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
    // Replace the entry if it exists.
    if(m_infoMap.count(name) != 0) {
      m_infoMap[name] = value;
    } else {
      m_infoMap.insert(name, value);
    }
  }

  //! Get a list for all of the tags
  inline QList<QString> getTagList() { return m_infoMap.keys(); }

  //! Check if a tag exists.
  inline bool tagExists(QString name) { return m_infoMap.count(name) != 0; }

  //! Get the value for a given tag.
  inline double getValue(QString name) {
    if (m_infoMap.count(name) != 0) {
      return m_infoMap.value(name);
    } else {
      return 0.0;
    }
  }

protected:
  QMap<QString, double> m_infoMap;

};

#endif // RTNAMEDINFOPOINTDATA_H
