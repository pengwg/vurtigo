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
#ifndef RT3DTIMEPOINTDATA_H
#define RT3DTIMEPOINTDATA_H

#include "rtBasic3DPointData.h"

class rt3DTimePointData : public rtBasic3DPointData
{
public:
  rt3DTimePointData();

  //! Copy constructor
  rt3DTimePointData(const rt3DTimePointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rt3DTimePointData &other) const;
  //! Assignment operator
  rt3DTimePointData& operator=(const rt3DTimePointData& sp);

  inline void setTriggerDelay(unsigned int td) { m_triggerDelay = td; }
  inline void setPhase(unsigned int phase) { m_phase = phase; }
  inline void setResp(unsigned int resp) { m_resp = resp; }

  inline unsigned int getTriggerDelay() { return m_triggerDelay; }
  inline unsigned int getPhase() { return m_phase; }
  inline unsigned int getResp() { return m_resp; }

protected:
  unsigned int m_triggerDelay;
  unsigned int m_phase;
  unsigned int m_resp;
};

#endif // RT3DTIMEPOINTDATA_H
