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
