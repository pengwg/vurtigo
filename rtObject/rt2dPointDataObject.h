#ifndef RT_2D_POINT_DATA_OBJECT_H
#define RT_2D_POINT_DATA_OBJECT_H

#include "rtDataObject.h"

//! Object that represents a set of 2D points
/*!
  @todo Implement this class
  */
class rt2DPointDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt2DPointDataObject();
  ~rt2DPointDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
