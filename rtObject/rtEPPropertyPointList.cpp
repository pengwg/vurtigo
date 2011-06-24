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
