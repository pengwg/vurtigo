#include "rt3DTimePointData.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

rt3DTimePointData::rt3DTimePointData()
 : m_triggerDelay(0), m_phase(0), m_resp(0)
{
  // Setup default to current time. They can be reset later if that is desired.
  m_triggerDelay = rtApplication::instance().getTimeManager()->getTriggerDelay();
  m_resp = rtApplication::instance().getTimeManager()->getGlobalResp();
}

rt3DTimePointData::rt3DTimePointData(const rt3DTimePointData& sp)
 : rtBasic3DPointData(sp)
{
  m_triggerDelay = sp.m_triggerDelay;
  m_phase = sp.m_phase;
  m_resp = sp.m_resp;
}


bool rt3DTimePointData::operator==(const rt3DTimePointData &other) const {
  return rtBasic3DPointData::operator==(other) && m_triggerDelay == other.m_triggerDelay && m_phase == other.m_phase && m_resp == other.m_resp;
}

rt3DTimePointData& rt3DTimePointData::operator=(const rt3DTimePointData& sp) {
  if (this!=&sp) {
    rtBasic3DPointData::operator=(sp);
    m_triggerDelay = sp.m_triggerDelay;
    m_phase = sp.m_phase;
    m_resp = sp.m_resp;
  }
  return *this;
}
