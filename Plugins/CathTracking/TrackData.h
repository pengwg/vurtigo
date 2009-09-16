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
#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include <QObject>
#include "rt2dSliceDataObject.h"
#include "rtCathDataObject.h"


class TrackData : public QObject
{
  Q_OBJECT

 public:
    TrackData(rtCathDataObject* cath, rt2DSliceDataObject* slice);
    ~TrackData();

    bool equivalentTo(rtCathDataObject* cath, rt2DSliceDataObject* slice);

    int getLocation() { return m_location; }
    void setLocation(int loc) { m_location = loc; }

    double getOffset() { return m_offset; }
    void setOffest(double offset) { m_offset = offset; }

    bool isTracking();
    void setTrackingOn();
    void setTrackingOff();
    void setTracking(bool t);

    rt2DSliceDataObject* getSliceObject() { return m_slice; }
    rtCathDataObject* getCathObject() { return m_cath; }

 public slots:
    //! Update is called when the catheter starts tracking and when it has moved.
    void update();

 protected:
  int m_location;
  double m_offset;
  bool m_tracking;
  rt2DSliceDataObject* m_slice;
  rtCathDataObject* m_cath;

  // Check if the catheter has really changed.
  int m_old_location;
  double m_old_offset;
  bool m_old_tracking;
  double m_old_position[3];

  double findDistance(double a[3], double b[3]);
};

#endif
