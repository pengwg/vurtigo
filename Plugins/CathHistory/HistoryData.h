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
#ifndef HISTORY_DATA_H
#define HISTORY_DATA_H

#include <QObject>
#include "rtCathDataObject.h"
#include "rt3dPointBufferDataObject.h"

class HistoryData : public QObject
{
  Q_OBJECT

 public:
    HistoryData(rtCathDataObject* cath, rt3DPointBufferDataObject* points);
    ~HistoryData();

    bool equivalentTo(rtCathDataObject* cath, rt3DPointBufferDataObject* points);
    
    void setCathObject(rtCathDataObject* cath) { m_cath = cath; }
    
    rtCathDataObject* getCathObject() { return m_cath; }
    
    rt3DPointBufferDataObject* getPointBufferObject() { return m_points; }
    
    void setAutoTrack(bool fEnabled);
    
    inline int getPointSize() { return m_pointSize; }
    inline void setPointSize(int sz) { if (sz > 0) m_pointSize = sz; }

 public slots:
    void savePoint();
    void saveSetPoint(int set);
    void doAutoTrack();

 protected:
  rtCathDataObject* m_cath;
  rt3DPointBufferDataObject* m_points;
  
  bool m_fAutoTrack;
  rtBasic3DPointData m_prevAutoTrackPoint;
  float m_autoTrackDistanceThreshold;

  int m_pointSize;
  
};

#endif
