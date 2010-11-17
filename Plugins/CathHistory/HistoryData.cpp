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
#include "HistoryData.h"
#include <iostream>
#include <cmath>

#include "rtBasic3DPointData.h"
#include "rt3DTimePointData.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

HistoryData::HistoryData(rtCathDataObject* cath, rt3DPointBufferDataObject* points)
 : m_pointSize(1)
{
  m_cath   = cath;
  m_points = points;
  
  m_fAutoTrack = false;

  m_prevAutoTrackPoint.setPoint(-1000, -1000, -1000);
  
  m_autoTrackDistanceThreshold = 2;
}

HistoryData::~HistoryData()
{
}

bool HistoryData::equivalentTo(rtCathDataObject* cath, rt3DPointBufferDataObject* points) {
  return (m_cath == cath && m_points == points);
}

void HistoryData::savePoint()
  {
   // fail if either object doesn't exist
    if (!m_cath || !m_points)
      return;
      
   // get (first) catheter position
    int        numLocs;
    QList<int> locs;

    numLocs = m_cath->getNumLocations();
    locs    = m_cath->getLocationList();

   // get position (Location starts at 1...)
    double pos[3];
    if (!m_cath->getPositionAtLocation(locs[0], pos))   /// xxxxxx should we use locs[1]?
      return;
      
   // create a point at this position
    rt3DTimePointData p;

    p.setPoint(pos);

    p.setPointSize(m_pointSize);

    p.setColor(1, 0, 0); // red
    p.getProperty()->SetOpacity(0.5);
    
    m_points->lock();
    m_points->addTimePoint(p);
    m_points->Modified();
    m_points->unlock();
  }


void HistoryData::setAutoTrack(bool fEnabled)
  {
    m_fAutoTrack = fEnabled;
    
    if (m_fAutoTrack)
      connect(m_cath, SIGNAL(objectChanged(int)), this, SLOT(doAutoTrack()));
    else
      disconnect(m_cath, SIGNAL(objectChanged(int)), this, SLOT(doAutoTrack()));
  }
  
void HistoryData::doAutoTrack()
  {
   // fail if either object doesn't exist
    if (!m_cath || !m_points)
      return;
      
   // get (first) catheter position
    int        numLocs;
    QList<int> locs;

    numLocs = m_cath->getNumLocations();
    locs    = m_cath->getLocationList();

   // get position (Location starts at 1...)
    double pos[3];
    if (!m_cath->getPositionAtLocation(locs[0], pos))   /// xxxxxx should we use locs[1]?
      return;
      
   // create a point at this position
    rt3DTimePointData p;
    
    p.setPoint(pos);

    if (rtBasic3DPointData::findDistance(p, m_prevAutoTrackPoint) < m_autoTrackDistanceThreshold)
      return;

    p.setPointSize(m_pointSize);

    p.setColor(1, 0, 0); // red
    p.getProperty()->SetOpacity(0.5);

    m_prevAutoTrackPoint = p;
    
    m_points->lock();
    m_points->addTimePoint(p);
    m_points->Modified();
    m_points->unlock();
  }

