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
#include "rtBasic3DPointData.h"

rtBasic3DPointData::rtBasic3DPointData()
{
  this->setDimention(3);
}

rtBasic3DPointData::~rtBasic3DPointData() {
}

rtBasic3DPointData::rtBasic3DPointData(const rtBasic3DPointData& sp)
 : rtBasicPointData(sp)
{
  m_coords[0] = sp.m_coords[0];
  m_coords[1] = sp.m_coords[1];
  m_coords[2] = sp.m_coords[2];
}

bool rtBasic3DPointData::operator==(const rtBasic3DPointData &other) const {
  return rtBasicPointData::operator==(other) && m_coords[0]==other.m_coords[0] && m_coords[1]==other.m_coords[1] && m_coords[2]==other.m_coords[2];
}

rtBasic3DPointData& rtBasic3DPointData::operator=(const rtBasic3DPointData& sp) {
  if (this!=&sp) {
    rtBasicPointData::operator=(sp);
    m_coords[0]=sp.m_coords[0];
    m_coords[1]=sp.m_coords[1];
    m_coords[2]=sp.m_coords[2];
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

void rtBasic3DPointData::setPoint(double x, double y, double z) {
  m_coords[0] = x;
  m_coords[1] = y;
  m_coords[2] = z;
}
  
void rtBasic3DPointData::setPoint(double p[3]) {
  m_coords[0] = p[0];
  m_coords[1] = p[1];
  m_coords[2] = p[2];
}

void rtBasic3DPointData::translate(double x, double y, double z) {
  m_coords[0] += x;
  m_coords[1] += y;
  m_coords[2] += z;
}

void rtBasic3DPointData::applyTransform(vtkTransform* t) {
  if (!t) return;

  double p[3];
  p[0] = m_coords[0];
  p[1] = m_coords[1];
  p[2] = m_coords[2];
  t->TransformPoint(p,p);
  m_coords[0] = p[0];
  m_coords[1] = p[1];
  m_coords[2] = p[2];
}

#define SQR(x) ((x)*(x))
  
double rtBasic3DPointData::findDistance(rtBasic3DPointData p1, rtBasic3DPointData p2) {
  return sqrt(SQR((p1.m_coords[0] - p2.m_coords[0])) + SQR((p1.m_coords[1] - p2.m_coords[1])) + SQR((p1.m_coords[2] - p2.m_coords[2])));
}
  

