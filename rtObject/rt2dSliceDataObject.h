#ifndef RT_2D_SLICE_DATA_OBJECT_H
#define RT_2D_SLICE_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"

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


  bool isDataValid() { return m_imgDataValid; }
  bool copyImageData2D(vtkImageData* img);
  vtkImageData* getRawData() { return m_imgData; }
  vtkImageData* getTransformedData() { return m_imgReslice->GetOutput(); }
  vtkTransform* getTransform() { return m_trans; }

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkImageData* m_imgData;
  vtkImageShiftScale * m_imgUCharCast;
  vtkImageReslice* m_imgReslice;
  vtkTransform* m_trans;

  bool m_imgDataValid;
};

#endif 
