#ifndef RT_3D_VOLUME_DATA_OBJECT_H
#define RT_3D_VOLUME_DATA_OBJECT_H

#include "rtDataObject.h"

//! 3D Volume Data Object
/*!
  @todo Implement this 3D Volume Data Object.
  */
class rt3DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt3DVolumeDataObject();
  ~rt3DVolumeDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
