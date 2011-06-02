#include "rtBasic2DPointData.h"

rtBasic2DPointData::rtBasic2DPointData()
{
  this->setDimention(2);
}

rtBasic2DPointData::rtBasic2DPointData(const rtBasic2DPointData& sp)
 : rtBasicPointData(sp)
{
  m_coords[0] = sp.m_coords[0];
  m_coords[1] = sp.m_coords[1];
}

bool rtBasic2DPointData::operator==(const rtBasic2DPointData &other) const {
  return rtBasicPointData::operator==(other) && m_coords[0]==other.m_coords[0] && m_coords[1]==other.m_coords[1];
}

rtBasic2DPointData& rtBasic2DPointData::operator=(const rtBasic2DPointData& sp) {
  if (this!=&sp) {
    rtBasicPointData::operator=(sp);
    m_coords[0]=sp.m_coords[0];
    m_coords[1]=sp.m_coords[1];
  }
  return *this;
}
