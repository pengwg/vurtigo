#ifndef RT_2D_SLICE_DATA_OBJECT_H
#define RT_2D_SLICE_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkMatrix4x4.h"
#include "vtkImageLuminance.h"

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
  bool setPlaneCenter(double center[3]);

  void pushPlaneBy(double amt);

  double getWindow() { return m_window; }
  double getLevel() { return m_level; }

 public slots:
  void spinRight();
  void spinLeft();
  void rotateUp();
  void rotateDown();
  void rotateLeft();
  void rotateRight();
  void pushPlane();
  void pullPlane();
  void xTranslate(double);
  void yTranslate(double);
  void zTranslate(double);

  void windowSliderMoved(int);
  void levelSliderMoved(int);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkImageData* m_imgData;
  vtkImageShiftScale * m_imgUCharCast;
  vtkTransform* m_trans;
  vtkImageLuminance* m_lumin;

  int m_FOV;
  int m_imgSize;
  int m_numChan;

  bool m_imgDataValid;

  double m_window;
  double m_level;
  double m_range[2];

  Ui::slice2DOptions m_optionsWidget;
};

#endif 
