#include "rtEPPropertyPointList.h"

rtEPPropertyPointList::rtEPPropertyPointList(QString name)
: m_maxPropValue(-1), m_minPropValue(-1)
{
  m_propName = name;
  m_infoList.clear();
}

rtEPPropertyPointList::~rtEPPropertyPointList() {
}


void rtEPPropertyPointList::addPointToList(rtNamedInfoPointData p) {
  if ( !p.tagExists(m_propName) ) return;

  if (m_infoList.empty()) {
    m_maxPropValue = p.getValue(m_propName);
    m_minPropValue = p.getValue(m_propName);
    m_infoList.append(p);
  } else {
    m_infoList.append(p);
    if (p.getValue(m_propName) > m_maxPropValue) m_maxPropValue = p.getValue(m_propName);
    if (p.getValue(m_propName) < m_minPropValue) m_minPropValue = p.getValue(m_propName);
  }
}

bool rtEPPropertyPointList::getInfoPoint(double x,double y,double z, rtNamedInfoPointData&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() && !found; ix1++) {
    if (m_infoList[ix1].getX() == x && m_infoList[ix1].getY() == y && m_infoList[ix1].getZ() == z) {
      p = m_infoList.at(ix1);
      found = true;
    }
  }
  return found;
}


bool rtEPPropertyPointList::removeInfoPoint(double x,double y,double z,rtNamedInfoPointData&p) {
  bool found=false;
  for (int ix1=0; ix1<m_infoList.size() &&!found; ix1++) {
    if (m_infoList[ix1].getX() == x && m_infoList[ix1].getY() == y && m_infoList[ix1].getZ() == z) {
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
