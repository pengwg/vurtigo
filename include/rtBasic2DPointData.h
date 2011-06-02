#ifndef RTBASIC2DPOINTDATA_H
#define RTBASIC2DPOINTDATA_H

#include "rtBasicPointData.h"

class rtBasic2DPointData : public rtBasicPointData
{
public:
  rtBasic2DPointData();

  //! Copy constructor
  rtBasic2DPointData(const rtBasic2DPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtBasic2DPointData &other) const;

  //! Assignment operator
  rtBasic2DPointData& operator=(const rtBasic2DPointData& sp);

  inline void setX(double x) { m_coords[0] = x; }
  inline void setY(double y) { m_coords[1] = y; }

  inline double getX() { return m_coords[0]; }
  inline double getY() { return m_coords[1]; }
};

#endif // RTBASIC2DPOINTDATA_H
