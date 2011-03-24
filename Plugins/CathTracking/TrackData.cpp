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
#include "TrackData.h"
#include <iostream>
#include <cmath>

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

TrackData::TrackData(rtCathDataObject* cath, rt2DSliceDataObject* slice)
{
  m_location = 0;
  m_offset = 0;
  m_tracking = false;
  m_cath = cath;
  m_slice = slice;
  m_vol = NULL;
  m_axial = false;
  m_sagittal = false;
  m_coronal = false;

  m_sliceOnly = false;

  m_old_location = 0;
  m_old_offset = 0.0;
  m_old_tracking = false;
  m_old_position[0] = 0.0f;
  m_old_position[1] = 0.0f;
  m_old_position[2] = 0.0f;
}

TrackData::TrackData(rtCathDataObject* cath, rt3DVolumeDataObject* vol)
{
  m_location = 0;
  m_offset = 0;
  m_tracking = false;
  m_cath = cath;
  m_slice = NULL;
  m_vol = vol;
  m_axial = false;
  m_sagittal = false;
  m_coronal = false;

  m_sliceOnly = false;

  m_old_location = 0;
  m_old_offset = 0.0;
  m_old_tracking = false;
  m_old_position[0] = 0.0f;
  m_old_position[1] = 0.0f;
  m_old_position[2] = 0.0f;
}


TrackData::~TrackData()
{
}

bool TrackData::equivalentCPTo(rtCathDataObject* cath, rt2DSliceDataObject* slice) {
  if (m_cath == cath && m_slice == slice) return true;
  return false;
}

bool TrackData::equivalentCVTo(rtCathDataObject* cath, rt3DVolumeDataObject* vol) {
  if (m_cath == cath && m_vol == vol) return true;
  return false;
}

bool TrackData::isTracking() {
  return m_tracking;
}

bool TrackData::isSliceOnly() {
  return m_sliceOnly;
}

void TrackData::setSliceOnly(bool value) {
  m_sliceOnly = value;
}

void TrackData::setPlanes(bool axial, bool sagittal, bool coronal)
{
    m_axial = axial;
    m_sagittal = sagittal;
    m_coronal = coronal;
}

void TrackData::setTrackingOn() {
  m_tracking = true;

  // When the catheter changes we update the plane.
  connect(m_cath, SIGNAL(objectChanged(int)), this, SLOT(update()));
}

void TrackData::setTrackingOff() {
  m_tracking = false;

  disconnect(m_cath,  SIGNAL(objectChanged(int)), this, SLOT(update()) );
}

void TrackData::setTracking(bool t) {
  m_tracking = t;

  if (m_tracking) {
    connect(m_cath, SIGNAL(objectChanged(int)), this, SLOT(update()));
  } else {
    disconnect(m_cath, SIGNAL(objectChanged(int)), this, SLOT(update()) );
  }

}

void TrackData::update() {
  if (!m_cath) return;

  bool skipUpdate = true;
  int numLoc;
  double pos[3];
  double pos_new[3];
  QList<int> locs;

  numLoc = m_cath->getNumLocations();
  if (numLoc == 0)
      return;

  locs = m_cath->getLocationList();

  // Ensure the location is in the proper bounds.
  if (m_location < 0) m_location = 0;
  m_location = m_location % numLoc;

  // Make sure we can get the position. (Location starts at 1...)
  if (!m_cath->getPositionAtLocation(locs[m_location], pos)) return;

  skipUpdate = skipUpdate && ( m_location == m_old_location );
  skipUpdate = skipUpdate && ( m_offset == m_old_offset );
  skipUpdate = skipUpdate && ( m_tracking == m_old_tracking );
  skipUpdate = skipUpdate && ( findDistance(m_old_position, pos) <= 1 );

// xxxxxxxxxxxxxxxxxxxxxxxxxxxx  if (skipUpdate) return;

//  std::cout << "TrackData::Update pos={" << pos[0] << " " << pos[1] << " " << pos[2] << "}" << "offset = " << m_offset << endl;
  if (m_slice)
  {
      if (m_sliceOnly)
          // find nearest point along line from old center in normal direction
      {
          double oldPos[3];
          double normal[3];

          m_slice->getPlaneCenter(oldPos);
          m_slice->getPlaneNormal(normal);

          // projection of point onto plane: P_proj = P_0 + [(P' - P_0 . N) / (N . N)] * N
          double d1[3];
          d1[0] = pos[0] - oldPos[0];
          d1[1] = pos[1] - oldPos[1];
          d1[2] = pos[2] - oldPos[2];

          double dd1 = d1[0] * normal[0] + d1[1] * normal[1] + d1[2] * normal[2];
          double dd2 = normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2];

          pos_new[0] = oldPos[0] + (dd1/dd2) * normal[0];
          pos_new[1] = oldPos[1] + (dd1/dd2) * normal[1];
          pos_new[2] = oldPos[2] + (dd1/dd2) * normal[2];
      }
      else
          // move plane center exactly to catheter tip
      {
          pos_new[0] = pos[0];
          pos_new[1] = pos[1];
          pos_new[2] = pos[2];
      }

      m_slice->setPlaneCenter(pos_new, true);    /* EthanB 2010-09-10 - switched asUser to true */
      m_slice->pushPlaneBy(m_offset, true);  /* EthanB 2010-09-10 - switched asUser to true */

      // Set the variables to "old" status after the update.
      m_old_location = m_location;
      m_old_offset = m_offset;
      m_old_tracking = m_tracking;
      m_old_position[0] = pos[0];
      m_old_position[1] = pos[1];
      m_old_position[2] = pos[2];
  }
  if (m_vol && m_tracking)
  {
      //cath position is pos
      // move plane to that point + offset
      m_vol->movePlanestoPoint(pos,m_offset,m_axial,m_sagittal,m_coronal);
  }
}

 double TrackData::findDistance(double a[3], double b[3]) {
   double diff[3];
   double totalSum;

   totalSum = 0.0;
   for (int ix1=0; ix1<3; ix1++) {
     diff[ix1] = a[ix1]-b[ix1];
     diff[ix1] = diff[ix1]*diff[ix1];
     totalSum += diff[ix1];
   }
   return sqrt(totalSum);
 }
