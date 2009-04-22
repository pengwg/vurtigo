#ifndef RT_CATH_DATA_OBJECT_H
#define RT_CATH_DATA_OBJECT_H

#include "rtDataObject.h"

//! Catheter Data Object
/*!
  @todo Implement this Catheter Data Object.
  */
class rtCathDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtCathDataObject();
  ~rtCathDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
