#ifndef RT_PIECE_FUNC_DATA_OBJECT_H
#define RT_PIECE_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"

//! Data Object from Piecewise Function
/*!
  @todo Implement this class
  */
class rtPieceFuncDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtPieceFuncDataObject();
  ~rtPieceFuncDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
