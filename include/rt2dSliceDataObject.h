/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RT_2D_SLICE_DATA_OBJECT_H
#define RT_2D_SLICE_DATA_OBJECT_H

#include <QSemaphore>
#include <QDateTime>

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

 public slots:
  void copyImageData2DSlot();
 signals:
  void copyImageData2DSignal();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkImageData* m_imgData;
  vtkImageShiftScale * m_imgUCharCast;
  vtkTransform* m_trans;
  vtkImageLuminance* m_lumin;

  QSemaphore m_dataCopyLock;

  int m_FOV;
  int m_imgSize;
  int m_numChan;

  bool m_imgDataValid;

  double m_window;
  double m_level;
  double m_range[2];

  Ui::slice2DOptions m_optionsWidget;

  QDateTime m_currTime;
};

#endif 
