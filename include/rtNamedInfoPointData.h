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
#ifndef RTNAMEDINFOPOINTDATA_H
#define RTNAMEDINFOPOINTDATA_H

#include <QString>
#include <QList>
#include <QMap>

#include "rtBasic3DPointData.h"

class rtCartoPointData;
class rt3DTimePointData;

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

  void fromBasic3DPoint(rtBasic3DPointData* pt);
  void fromTimePoint(rt3DTimePointData* pt);
  void fromCartoPoint(rtCartoPointData* pt);

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

  inline QString getLabel() { return m_label;}
  inline void setLabel(QString label) {m_label = label;}

protected:
  QMap<QString, double> m_infoMap;
  // The point's text label
  QString m_label;

};

#endif // RTNAMEDINFOPOINTDATA_H
