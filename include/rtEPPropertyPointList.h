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
