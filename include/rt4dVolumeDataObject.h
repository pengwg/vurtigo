#ifndef RT_4D_VOLUME_DATA_OBJECT_H
#define RT_4D_VOLUME_DATA_OBJECT_H

#include "rtDataObject.h"

//! 3D Volume Plus Time
/*!
  @todo Class Not Implemented
 */
class rt4DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt4DVolumeDataObject();
  ~rt4DVolumeDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
