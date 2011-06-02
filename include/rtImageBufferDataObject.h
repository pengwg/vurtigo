#ifndef RT_IMAGE_BUFFER_OBJECT_H
#define RT_IMAGE_BUFFER_OBJECT_H

#include "rtDataObject.h"

//! Object that represents a set of 2D images
/*!
  @todo Implement this class
  */
class rtImageBufferDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtImageBufferDataObject();
  ~rtImageBufferDataObject();

  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
