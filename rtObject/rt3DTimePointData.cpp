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
