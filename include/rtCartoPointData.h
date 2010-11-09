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
