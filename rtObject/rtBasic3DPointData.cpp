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
  

