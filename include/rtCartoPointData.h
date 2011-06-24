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
#ifndef RTCARTOPOINTDATA_H
#define RTCARTOPOINTDATA_H

#include "rt3DTimePointData.h"
#include "rtNamedInfoPointData.h"

//! The size of the LAT array
#define RT_CARTO_POINT_DATA_LAT_SIZE 10

class rtCartoPointData : public rt3DTimePointData
{
public:

  //! Default constructor
  rtCartoPointData();

  //! Copy constructor
  rtCartoPointData(const rtCartoPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtCartoPointData &other) const;
  //! Assignment operator
  rtCartoPointData& operator=(const rtCartoPointData& sp);

  //! Convert to named info but keep only the bi-polar info.
  rtNamedInfoPointData toNamedInfoBiPolar();

  //! Convert to named into and keep all the tags.
  rtNamedInfoPointData toNamedInfo();

  inline void setAlpha(double a) { m_alpha = a; }
  inline void setBeta(double b) { m_beta = b; }
  inline void setGamma(double c) { m_gamma = c; }
  inline void setUniPolar(double uniPolar) { m_uniPolar = uniPolar; }
  inline void setBiPolar(double biPolar) { m_biPolar = biPolar; }
  inline void setLAT(int lat) { m_LAT = lat; }
  inline void setLATArray(int lat, int pos) { if (pos >= 0 && pos < RT_CARTO_POINT_DATA_LAT_SIZE) m_LATArray[pos] = lat; }
  inline void setPointNumber(int ptNum) { m_pointNum = ptNum; }

  inline double getAlpha() { return m_alpha; }
  inline double getBeta() { return m_beta; }
  inline double getGamma() { return m_gamma; }
  inline double getUniPolar() { return m_uniPolar; }
  inline double getBiPolar() { return m_biPolar; }
  inline int getLAT() { return m_LAT; }
  inline int getLATArray(int pos) {
    if( pos >=0 && pos < RT_CARTO_POINT_DATA_LAT_SIZE )  return m_LATArray[pos];
    return 0;
  }
  inline int getPointNumber() { return m_pointNum; }

protected:
  double m_alpha, m_beta, m_gamma;
  double m_uniPolar;
  double m_biPolar;
  int m_LAT;
  int m_LATArray[RT_CARTO_POINT_DATA_LAT_SIZE];

  //! The point number from the custom XML file
  int m_pointNum;
};

#endif // RTCARTOPOINTDATA_H
