#ifndef RT_NONE_DATA_OBJECT_H
#define RT_NONE_DATA_OBJECT_H

#include "rtDataObject.h"

//! Simplest form of Data Object
class rtNoneDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtNoneDataObject();
  ~rtNoneDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
