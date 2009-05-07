#ifndef RT_MATRIX_DATA_OBJECT_H
#define RT_MATRIX_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkMatrix4x4.h"

//! Matrix Object
/*!
  @todo Implement this object
  */
class rtMatrixDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtMatrixDataObject();
  ~rtMatrixDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  vtkMatrix4x4 *m_currMatrix;
  
};

#endif 
