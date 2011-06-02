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
