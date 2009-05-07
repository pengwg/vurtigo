#ifndef RT_POLY_DATA_OBJECT_H
#define RT_POLY_DATA_OBJECT_H

#include "rtDataObject.h"

//! Object that represents polygonal data
/*!
  @todo Implement this class
  */
class rtPolyDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtPolyDataObject();
  ~rtPolyDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
