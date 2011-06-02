#include "rtCardiacMeshPointData.h"

rtCardiacMeshPointData::rtCardiacMeshPointData()
 : m_location(0), m_slice(0)
{
}

rtCardiacMeshPointData::rtCardiacMeshPointData(const rtCardiacMeshPointData& sp)
 : rt3DTimePointData(sp)
{
  m_location = sp.m_location;
  m_slice = sp.m_slice;
}

bool rtCardiacMeshPointData::operator==(const rtCardiacMeshPointData &other) const {
  return rt3DTimePointData::operator==(other) && m_location == other.m_location && m_slice == other.m_slice;
}

rtCardiacMeshPointData& rtCardiacMeshPointData::operator=(const rtCardiacMeshPointData& sp) {
  if (this!=&sp) {
    rt3DTimePointData::operator=(sp);
    m_location = sp.m_location;
    m_slice = sp.m_slice;
  }
  return *this;
}
