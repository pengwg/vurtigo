#include "rtNamedInfoPointData.h"
#include "rtCartoPointData.h"
#include "rt3DTimePointData.h"

rtNamedInfoPointData::rtNamedInfoPointData()
{
  m_infoMap.clear();
  m_label = "";
}

rtNamedInfoPointData::rtNamedInfoPointData(const rtNamedInfoPointData& sp)
 : rtBasic3DPointData(sp)
{
  m_infoMap = sp.m_infoMap;
  m_label = sp.m_label;
}

bool rtNamedInfoPointData::operator==(const rtNamedInfoPointData &other) const {
  return rtBasic3DPointData::operator==(other) && m_infoMap == other.m_infoMap;
}

rtNamedInfoPointData& rtNamedInfoPointData::operator=(const rtNamedInfoPointData& sp) {
  if (this!=&sp) {
    rtBasic3DPointData::operator=(sp);
    m_infoMap = sp.m_infoMap;
  }
  return *this;
}

void rtNamedInfoPointData::fromBasic3DPoint(rtBasic3DPointData* pt) {
  rtBasic3DPointData::operator=( (*pt) );
}

void rtNamedInfoPointData::fromTimePoint(rt3DTimePointData* pt) {
  rtBasic3DPointData::operator=( (*pt) );
  setNamedValue("Trigger Delay", pt->getTriggerDelay());
  setNamedValue("Resp", pt->getResp());
}

void rtNamedInfoPointData::fromCartoPoint(rtCartoPointData* pt) {
  rtBasic3DPointData::operator=( (*pt) );
  setNamedValue("Trigger Delay", pt->getTriggerDelay());
  setNamedValue("Resp", pt->getResp());
  setNamedValue("Alpha", pt->getAlpha());
  setNamedValue("Beta", pt->getBeta());
  setNamedValue("Gamma", pt->getGamma());
  setNamedValue("UniPolar", pt->getUniPolar());
  setNamedValue("BiPolar", pt->getBiPolar());
  setNamedValue("LAT", pt->getLAT());
  for (int ix1=0; ix1<RT_CARTO_POINT_DATA_LAT_SIZE; ix1++) {
    setNamedValue(QString("LATv") + QString::number(ix1), pt->getLATArray(ix1));
  }
  setNamedValue("Point Number", pt->getPointNumber());
}
