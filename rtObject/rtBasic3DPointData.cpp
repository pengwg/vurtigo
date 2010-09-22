/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "rtBasic3DPointData.h"

rtBasic3DPointData::rtBasic3DPointData()
 : m_pTransform(0)
{
  this->setDimention(3);
  m_pTransform = vtkTransform::New();
}

rtBasic3DPointData::~rtBasic3DPointData() {
  if (m_pTransform) m_pTransform->Delete();
}

rtBasic3DPointData::rtBasic3DPointData(const rtBasic3DPointData& sp)
 : rtBasicPointData(sp)
{
  m_coords[0] = sp.m_coords[0];
  m_coords[1] = sp.m_coords[1];
  m_coords[2] = sp.m_coords[2];

  m_pTransform = vtkTransform::New();
  m_pTransform->DeepCopy(sp.m_pTransform);
}

bool rtBasic3DPointData::operator==(const rtBasic3DPointData &other) const {
  return rtBasicPointData::operator==(other) && m_coords[0]==other.m_coords[0] && m_coords[1]==other.m_coords[1] && m_coords[2]==other.m_coords[2] && m_pTransform == other.m_pTransform;
}

rtBasic3DPointData& rtBasic3DPointData::operator=(const rtBasic3DPointData& sp) {
  if (this!=&sp) {
    rtBasicPointData::operator=(sp);
    m_coords[0]=sp.m_coords[0];
    m_coords[1]=sp.m_coords[1];
    m_coords[2]=sp.m_coords[2];

    if(m_pTransform) m_pTransform->Delete();
    m_pTransform = vtkTransform::New();
    m_pTransform->DeepCopy(sp.m_pTransform);
  }
  return *this;
}

double rtBasic3DPointData::getX() {
  return m_coords[0];
}

double rtBasic3DPointData::getY() {
  return m_coords[1];
}

double rtBasic3DPointData::getZ() {
  return m_coords[2];
}

void rtBasic3DPointData::getPoint(double p[3]) {
  p[0] = m_coords[0];
  p[1] = m_coords[1];
  p[2] = m_coords[2];
}

void rtBasic3DPointData::getTransformedPoint(double p[3]) {
  p[0] = m_coords[0];
  p[1] = m_coords[1];
  p[2] = m_coords[2];
  m_pTransform->TransformPoint(p, p);
}

void rtBasic3DPointData::setPoint(double x, double y, double z)
  {
    m_coords[0] = x;
    m_coords[1] = y;
    m_coords[2] = z;
  }
  
void rtBasic3DPointData::setPoint(double p[3])
  {
    m_coords[0] = p[0];
    m_coords[1] = p[1];
    m_coords[2] = p[2];
  }
  
#define SQR(x) ((x)*(x))
  
double rtBasic3DPointData::findDistance(rtBasic3DPointData p1, rtBasic3DPointData p2)
  {
    return sqrt(SQR((p1.m_coords[0] - p2.m_coords[0])) + SQR((p1.m_coords[1] - p2.m_coords[1])) + SQR((p1.m_coords[2] - p2.m_coords[2])));
  }
  

