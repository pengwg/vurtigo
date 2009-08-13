#ifndef RT_2D_SLICE_DATA_OBJECT_H
#define RT_2D_SLICE_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkMatrix4x4.h"

#include "ui_slice2DOptions.h"

#include <vector>

//! A Single 2D Slice
/*!
  The data object for a single 2D slice.
  */
class rt2DSliceDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt2DSliceDataObject();
  ~rt2DSliceDataObject();

  void update();

  bool isDataValid() { return m_imgDataValid; }
  bool copyImageData2D(vtkImageData* img);
  vtkImageData* getRawData() { return m_imgData; }
  vtkImageData* getUCharData() { return m_imgUCharCast->GetOutput(); }
  vtkTransform* getTransform() { return m_trans; }

  bool setTransform(float rotMatrix[9], float transMatrix[3]);
  bool setVtkMatrix(vtkMatrix4x4* m);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkImageData* m_imgData;
  vtkImageShiftScale * m_imgUCharCast;
  vtkTransform* m_trans;

  int m_FOV;
  int m_imgSize;
  int m_numChan;

  bool m_imgDataValid;

  Ui::slice2DOptions m_optionsWidget;
};

#endif 
