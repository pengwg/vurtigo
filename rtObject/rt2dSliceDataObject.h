#ifndef RT_2D_SLICE_DATA_OBJECT_H
#define RT_2D_SLICE_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

//! A Single 2D Slice
/*!
  @todo Implement this class
  */
class rt2DSliceDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt2DSliceDataObject();
  ~rt2DSliceDataObject();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkImageData* m_imgData;
  vtkTransformFilter* m_transFilter;
  vtkTransform* m_trans;
};

#endif 
