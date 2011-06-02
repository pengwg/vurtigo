#ifndef RTCARDIACMESHPOINTDATA_H
#define RTCARDIACMESHPOINTDATA_H

#include "rt3DTimePointData.h"

class rtCardiacMeshPointData : public rt3DTimePointData
{
public:
  rtCardiacMeshPointData();

  //! Copy constructor
  rtCardiacMeshPointData(const rtCardiacMeshPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtCardiacMeshPointData &other) const;
  //! Assignment operator
  rtCardiacMeshPointData& operator=(const rtCardiacMeshPointData& sp);


  inline void setLocation(unsigned int loc) { m_location=loc; }
  inline void setSlice(unsigned int slice) { m_slice = slice; }

  inline unsigned int getLocation() { return m_location; }
  inline unsigned int getSlice() { return m_slice; }

protected:
  //! Location order of the point on the given slice.
  unsigned int m_location;

  //! Slice number that this point lies on.
  unsigned int m_slice;
};

#endif // RTCARDIACMESHPOINTDATA_H
