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

#ifndef RTBASICPOINTDATA_H
#define RTBASICPOINTDATA_H

#include <vector>

#include <QColor>

#include <vtkProperty.h>

//! Generic data object for a point in N dimensional space.
class rtBasicPointData
{
public:
  //! Constructor
  rtBasicPointData();

  //! Copy constructor
  rtBasicPointData(const rtBasicPointData& sp);

  //! Destructor
  ~rtBasicPointData();

  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rtBasicPointData &other) const;

  //! Assignment operator
  rtBasicPointData& operator=(const rtBasicPointData& sp);

  inline int getPointId() { return m_pId; }
  inline void setPointId(int id) { m_pId = id; }

  inline double getPointSize() { return m_pSize; }
  inline void setPointSize(double sz) { if (sz > 0) m_pSize = sz; }

  inline std::vector<double> getCoordVector() { return m_coords; }

  inline unsigned int getDimension() { return m_coords.size(); }
  inline void setDimention(unsigned int dim) { m_coords.resize(dim, 0.0); }

  inline vtkProperty* getProperty() { return m_pProp; }

  //! Set the r,g,b color for this point.
  /*!
    Values for r,g,b should be in the range [0,1].
    */
  inline void setColor(double r, double g, double b) { m_pProp->SetColor(r,g,b); }

  inline void setColor(QColor c) { m_pProp->SetColor(c.redF(),c.greenF(),c.blueF()); }

  inline void getColor(double &r, double &g, double &b) { m_pProp->GetColor(r,g,b); }

  inline QColor getColor() {
    double r,g,b;
    QColor res;
    m_pProp->GetColor(r,g,b);
    res.setRedF(r);
    res.setGreenF(g);
    res.setBlueF(b);
    return res;
  }

protected:
  //! Point Id.
  int m_pId;
  double m_pSize;
  std::vector<double> m_coords;
  vtkProperty *m_pProp;
};

#endif // RTBASICPOINTDATA_H
