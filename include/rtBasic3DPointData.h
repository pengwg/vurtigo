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
