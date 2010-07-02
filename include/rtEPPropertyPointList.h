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

#ifndef RTEPPROPERTYPOINTLIST_H
#define RTEPPROPERTYPOINTLIST_H

#include <QString>
#include <QList>

#include "rtNamedInfoPointData.h"

class rtEPPropertyPointList
{
 public:

  //! Constructor
  rtEPPropertyPointList(QString name);
  ~rtEPPropertyPointList();

  //! Add an info point to this list.
  /*!
    Adding a point will update the min and max values for this property.
    */
  void addPointToList(rtNamedInfoPointData p);

  //! Get the point at the specified set of 3d coordinates
  /*!
    \param x The x coord for the search point.
    \param y The y coord for the search point.
    \param z The z coord for the search point.
    \param p The point that is found will be stored in p.
    \return True if a point is found at that location. False otherwise.
    */
  bool getInfoPoint(double x,double y,double z,rtNamedInfoPointData& p);

  bool removeInfoPoint(double,double,double,rtNamedInfoPointData&);

  void clearPointList();

  //! Get the number of points in this list.
  int getNumPoints() { return m_infoList.size(); }

  double getMinValue() { return m_minPropValue; }
  double getMaxValue() { return m_maxPropValue; }

  //! Get the point at a position in the list.
  rtNamedInfoPointData getPointAt(int pos) { return m_infoList.at(pos); }

 protected:

  //! The name of this property
  QString m_propName;
  //! List of points that hold EP info
  QList<rtNamedInfoPointData> m_infoList;
  //! Max value of the property
  double m_maxPropValue;
  //! Minimum value of the property
  double m_minPropValue;
};

#endif // RTEPPROPERTYPOINTLIST_H
