#include "rtEPPropertyPointList.h"

rtEPPropertyPointList::rtEPPropertyPointList(QString name)
: m_maxPropValue(-1), m_minPropValue(-1)
{
  m_propName = name;
  m_infoList.clear();
}

rtEPPropertyPointList::~rtEPPropertyPointList() {
}


void rtEPPropertyPointList::addPointToList(InfoPoint p) {
  if (m_infoList.empty()) {
    m_maxPropValue = p.property;
    m_minPropValue = p.property;
    m_infoList.append(p);
  } else {
    m_infoList.append(p);
    if (p.property > m_maxPropValue) m_maxPropValue = p.property;
    if (p.property < m_minPropValue) m_minPropValue = p.property;
  }
}

bool rtEPPropertyPointList::getInfoPoint(double x,double y,double z, InfoPoint&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() && !found; ix1++) {
    if (m_infoList.at(ix1).location[0] == x && m_infoList.at(ix1).location[1] == y && m_infoList.at(ix1).location[2] == z) {
      p = m_infoList.at(ix1);
      found = true;
    }
  }
  return found;
}


bool rtEPPropertyPointList::removeInfoPoint(double x,double y,double z,InfoPoint&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() &&!found; ix1++) {
    if (m_infoList.at(ix1).location[0] == x && m_infoList.at(ix1).location[1] == y && m_infoList.at(ix1).location[2] == z) {
      p = m_infoList.takeAt(ix1);
      found = true;
    }
  }
  return found;
}


void rtEPPropertyPointList::clearPointList() {
  m_infoList.clear();
  m_maxPropValue = -1;
  m_minPropValue = -1;
}
