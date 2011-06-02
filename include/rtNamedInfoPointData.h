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
