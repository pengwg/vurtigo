#ifndef RT_2D_POINT_DATA_OBJECT_H
#define RT_2D_POINT_DATA_OBJECT_H

#include "rtDataObject.h"
#include "rtBasic2DPointData.h"

//! Object that represents a set of 2D points
/*!
  A data representation of a point in 2D space.
  */
class rt2DPointDataObject : public rtDataObject
{
Q_OBJECT

public:

  rt2DPointDataObject();
  ~rt2DPointDataObject();

  QList<rtBasic2DPointData>* getPointList() { return &m_pointList; }
  void addPoint(rtBasic2DPointData sp);
  void removePoint(rtBasic2DPointData sp);

  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  QList<rtBasic2DPointData> m_pointList;
};

#endif 
