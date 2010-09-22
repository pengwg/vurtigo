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
#include <QList>

#include "rtDataObject.h"
#include "rtWindowLevelDialog.h"
#include "rt2dSliceInputColorWidget.h"

#include <vtkImageData.h>
#include <vtkTransform.h>
#include <vtkImageReslice.h>
#include <vtkImageShiftScale.h>
#include <vtkMatrix4x4.h>
#include <vtkImageLuminance.h>

#include "ui_slice2DOptions.h"

//! A Single 2D Slice
/*!
  The data object for a single 2D slice.
  This object has two modes. A standard updating mode and a special manual user mode. In manual mode the plane is supposed to receive instructions directly from the Vurtigo user and as such instructions from the update mode are ignored.
  If the Qt group box: m_optionsWidget.prescribeGroupBox is selected then the object is in manual mode. The group box is also checked when the user selects the object in the 3D view. Plugins should not use the manual mode as this may interfere with actions performed by the user when they try to control the plane by hand.
  */
class rt2DSliceDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt2DSliceDataObject();
  ~rt2DSliceDataObject();

  void update();

  //! Get the maximum number of inputs to the 2D plane.
  /*!
    This is a constant return value.
    */
  inline unsigned short maxNumberOfInputs() { return 3; }

  //! Check if the image data is valid.
  /*!
    Invalid image data has not been loaded and an attempt to render invalid data results in access to unallocated memory space.
    */
  inline bool isDataValid() { return m_imgDataValid; }

  //! Copy new data over top of this one.
  /*!
  */
  bool copyImageData2D(vtkImageData* img);

  inline vtkImageData* getRawData() { return m_imgData; }
  inline vtkImageData* getUCharData() { return m_imgUCharCast->GetOutput(); }
  inline vtkTransform* getTransform() { return m_trans; }

  bool setTransform(float rotMatrix[9], float transMatrix[3], bool asUser=false);
  bool setVtkMatrix(vtkMatrix4x4* m, bool asUser=false);


  bool setPlaneCenter(double center[3], bool asUser=false);
  bool setPlaneNormal(double normal[3], bool asUser=false);
  
  void getPlaneCenter(double center[3]);
  void getPlaneNormal(double normal[3]);

  //! Push the plane in the direction of the surface normal.
  /*!
    \param amt The amount to push by. A negative value will push the plane in the opposite direction.
    \param asUser Push the plane as a user. If this is set to true the plane will move even in user mode.
    */
  void pushPlaneBy(double amt, bool asUser=false);


  //! Spin the plane to the left.
  void spinLeftBy(double amt, bool asUser=false);

  //! Rotate the plane up
  /*!
    \param amt The amount to rotate by. If negative this function will rotate down.
    \param asUser If true rotate in user mode.
    */
  void rotateUpBy(double amt, bool asUser=false);

  //! Rotate the plane left
  void rotateLeftBy(double amt, bool asUser=false);

  //! Translate the object to a specific position
  void translateTo(double x, double y, double z, bool asUser=false);


  //! Get the wndow part of the window level
  inline double getWindow() { return m_window; }
  //! Get the level part of the window level
  inline double getLevel() { return m_level; }

  inline void setManualOn() { m_optionsWidget.prescribeGroupBox->setChecked(true); }
  inline void setManualOff() { m_optionsWidget.prescribeGroupBox->setChecked(false); }
  inline void setManual(bool man) { m_optionsWidget.prescribeGroupBox->setChecked(man); }
  inline bool getManual() { return m_optionsWidget.prescribeGroupBox->isChecked(); }

  //! Get the pointer to one of the three color widgets
  inline rt2DSliceInputColorWidget* getColorWidgetAt(int pos) { if ( pos <0 || pos>=maxNumberOfInputs() ) return 0; return m_colorInput[pos]; }

 protected slots:
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

  void wlChanged(int w, int l);

  void copyImageData2DSlot();

  void showWindowLevel();

 signals:
  void copyImageData2DSignal();

 protected:
  // Functions

  void setupGUI();
  //! Clean the GUI widgets.
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

  Ui::slice2DOptions m_optionsWidget;

  QList<rt2DSliceInputColorWidget*> m_colorInput;

  QDateTime m_currTime;

  rtWindowLevelDialog *m_wlDialog;
};

#endif 
