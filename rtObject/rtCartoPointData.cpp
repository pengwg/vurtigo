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
