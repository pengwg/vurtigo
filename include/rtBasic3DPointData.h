#ifndef RTBASIC3DPOINTDATA_H
#define RTBASIC3DPOINTDATA_H

#include <vtkTransform.h>

#include "rtBasicPointData.h"

class rtBasic3DPointData : public rtBasicPointData
{
public:
  rtBasic3DPointData();
  ~rtBasic3DPointData();

  //! Copy constructor
  rtBasic3DPointData(const rtBasic3DPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtBasic3DPointData &other) const;
  //! Assignment operator
  rtBasic3DPointData& operator=(const rtBasic3DPointData& sp);

  inline void setX(double x) { m_coords[0] = x; }
  inline void setY(double y) { m_coords[1] = y; }
  inline void setZ(double z) { m_coords[2] = z; }

  double getX();
  double getY();
  double getZ();
  void getPoint(double p[3]);
  
  void setPoint(double x, double y, double z);
  void setPoint(double p[3]);

  //! Translate the given point.
  /*!
    For more general manipulation of the point use the applyTransform function. If all that is required is a translation use this function as it is more efficient than a transform.
    \param x The translation in x.
    \param y The translation in y.
    \param z The translation in z.
    \sa applyTransform()
    */
  void translate(double x, double y, double z);

  //! Apply a given transform to this point.
  /*!
    Used as a general way to manipulate the point. If all that is required is a translation then use the translate() function.
    \sa translate()
    */
  void applyTransform(vtkTransform* t);
  
  static double findDistance(rtBasic3DPointData p1, rtBasic3DPointData p2);
  
protected:

};

#endif // RTBASIC3DPOINTDATA_H
