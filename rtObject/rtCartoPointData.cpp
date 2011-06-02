#include "rtCartoPointData.h"

rtCartoPointData::rtCartoPointData()
 : m_alpha(0.0), m_beta(0.0), m_gamma(0.0), m_uniPolar(0.0), m_biPolar(0.0), m_LAT(0), m_pointNum(0)
{
  for (int ix1=0; ix1<RT_CARTO_POINT_DATA_LAT_SIZE; ix1++) {
    m_LATArray[ix1] = 0;
  }
}

rtCartoPointData::rtCartoPointData(const rtCartoPointData& sp)
 : rt3DTimePointData(sp)
{
  m_alpha = sp.m_alpha;
  m_beta = sp.m_beta;
  m_gamma = sp.m_gamma;
  m_uniPolar = sp.m_uniPolar;
  m_biPolar = sp.m_biPolar;
  m_LAT = sp.m_LAT;
  m_pointNum = sp.m_pointNum;
  for (int ix1=0; ix1<RT_CARTO_POINT_DATA_LAT_SIZE; ix1++) {
    m_LATArray[ix1] = sp.m_LATArray[ix1];
  }
}

bool rtCartoPointData::operator==(const rtCartoPointData &other) const {
  bool res=rt3DTimePointData::operator==(other);

  res = res && m_alpha == other.m_alpha;
  res = res && m_beta == other.m_beta;
  res = res && m_gamma == other.m_gamma;
  res = res && m_uniPolar == other.m_uniPolar;
  res = res && m_biPolar == other.m_biPolar;
  res = res && m_LAT == other.m_LAT;
  res = res && m_pointNum == other.m_pointNum;
  for (int ix1=0; ix1<RT_CARTO_POINT_DATA_LAT_SIZE; ix1++) {
    res = res && m_LATArray[ix1] == other.m_LATArray[ix1];
  }
  return res;
}

rtCartoPointData& rtCartoPointData::operator=(const rtCartoPointData& sp) {
  if (this!=&sp) {
    rt3DTimePointData::operator=(sp);
    m_alpha = sp.m_alpha;
    m_beta = sp.m_beta;
    m_gamma = sp.m_gamma;
    m_uniPolar = sp.m_uniPolar;
    m_biPolar = sp.m_biPolar;
    m_LAT = sp.m_LAT;
    m_pointNum = sp.m_pointNum;
    for (int ix1=0; ix1<RT_CARTO_POINT_DATA_LAT_SIZE; ix1++) {
      m_LATArray[ix1] = sp.m_LATArray[ix1];
    }
  }
  return *this;
}

rtNamedInfoPointData rtCartoPointData::toNamedInfoBiPolar() {
  rtNamedInfoPointData res;

  res.setPointId(this->getPointId());
  res.setX(this->getX());
  res.setY(this->getY());
  res.setZ(this->getZ());
  res.setPointSize(this->getPointSize());
  res.getProperty()->DeepCopy(this->getProperty());
  res.setNamedValue("Bi-Polar", this->getBiPolar());
  return res;
}

rtNamedInfoPointData rtCartoPointData::toNamedInfo() {
  rtNamedInfoPointData res;

  res.setPointId(this->getPointId());
  res.setX(this->getX());
  res.setY(this->getY());
  res.setZ(this->getZ());
  res.setPointSize(this->getPointSize());
  res.getProperty()->DeepCopy(this->getProperty());
  res.setNamedValue("Trigger Delay", this->getTriggerDelay());
  res.setNamedValue("Resp", this->getResp());
  res.setNamedValue("Point Number", this->getPointNumber());
  res.setNamedValue("Alpha", this->getAlpha());
  res.setNamedValue("Beta", this->getBeta());
  res.setNamedValue("Gamma", this->getGamma());
  res.setNamedValue("Uni-Polar", this->getUniPolar());
  res.setNamedValue("Bi-Polar", this->getBiPolar());
  res.setNamedValue("LAT", this->getLAT());
  return res;
}
