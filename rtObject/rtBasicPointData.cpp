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
#include <vtkProperty.h>

#include "rtBasicPointData.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

rtBasicPointData::rtBasicPointData()
 : m_pId(-1), m_pSize(1.0), m_pProp(0)
{
  m_coords.clear();
  m_pProp = vtkProperty::New();
  m_pProp->SetColor(0.75, 0.75, 0.75);
  m_pProp->SetOpacity(1.0);

  m_selectedProp = vtkProperty::New();
  m_selectedProp->SetColor(1.0, 0.0, 1.0);
  m_selectedProp->SetOpacity(0.5);

  m_timeStamp = rtApplication::instance().getTimeManager()->getSystemTime();
}

rtBasicPointData::rtBasicPointData(const rtBasicPointData& sp) {
  m_pId = sp.m_pId;
  m_coords = sp.m_coords;
  m_pSize = sp.m_pSize;
  m_timeStamp = sp.m_timeStamp;
  m_pProp = vtkProperty::New();
  m_pProp->DeepCopy(sp.m_pProp);
  m_selectedProp = vtkProperty::New();
  m_selectedProp->DeepCopy(sp.m_selectedProp);
}

rtBasicPointData::~rtBasicPointData() {
  if(m_pProp) m_pProp->Delete();
  if(m_selectedProp) m_selectedProp->Delete();
}

bool rtBasicPointData::operator==(const rtBasicPointData &other) const {
  if (m_pId==other.m_pId &&  m_coords==other.m_coords && m_pSize == other.m_pSize && m_pProp == other.m_pProp && m_selectedProp == other.m_selectedProp && m_timeStamp==other.m_timeStamp)
    return true;
  else
    return false;
}

rtBasicPointData& rtBasicPointData::operator=(const rtBasicPointData& sp) {
  if (this != &sp) {
    m_pId = sp.m_pId;
    m_coords = sp.m_coords;
    m_pSize = sp.m_pSize;
    m_timeStamp = sp.m_timeStamp;

    if(m_pProp) m_pProp->Delete();
    m_pProp = vtkProperty::New();
    m_pProp->DeepCopy(sp.m_pProp);

    if(m_selectedProp) m_selectedProp->Delete();
    m_selectedProp = vtkProperty::New();
    m_selectedProp->DeepCopy(sp.m_selectedProp);
  }

  return *this;
}
