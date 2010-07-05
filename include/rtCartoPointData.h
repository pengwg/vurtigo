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

class rtCartoPointData : public rt3DTimePointData
{

public:
  rtCartoPointData();

  //! Copy constructor
  rtCartoPointData(const rtCartoPointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtCartoPointData &other) const;
  //! Assignment operator
  rtCartoPointData& operator=(const rtCartoPointData& sp);

  inline void setAlpha(double a) { m_alpha = a; }
  inline void setBeta(double b) { m_beta = b; }
  inline void setGamma(double c) { m_gamma = c; }
  inline void setUniPolar(double uniPolar) { m_uniPolar = uniPolar; }
  inline void setBiPolar(double biPolar) { m_biPolar = biPolar; }
  inline void setLAT(int lat) { m_LAT = lat; }

  inline double getAlpha() { return m_alpha; }
  inline double getBeta() { return m_beta; }
  inline double getGamma() { return m_gamma; }
  inline double getUniPolar() { return m_uniPolar; }
  inline double getBiPolar() { return m_biPolar; }
  inline double getLAT() { return m_LAT; }

protected:
  double m_alpha, m_beta, m_gamma;
  double m_uniPolar;
  double m_biPolar;
  int m_LAT;
};

#endif // RTCARTOPOINTDATA_H
