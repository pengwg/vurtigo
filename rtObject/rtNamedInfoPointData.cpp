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
