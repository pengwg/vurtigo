#ifndef RT_COLOR_FUNC_DATA_OBJECT_H
#define RT_COLOR_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"


//! The data portion of the color function
/*!
  @todo Implement this class
  */
class rtColorFuncDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtColorFuncDataObject();
  ~rtColorFuncDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
