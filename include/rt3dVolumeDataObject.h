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
#ifndef RT_3D_VOLUME_DATA_OBJECT_H
#define RT_3D_VOLUME_DATA_OBJECT_H

#include "rtDataObject.h"
#include "ui_volume3DOptions.h"
#include "rtWindowLevelDialog.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkImageShiftScale.h"
#include "vtkImageExtractComponents.h"

#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"

#include <QTimer>

//! 3D Volume Data Object
/*!
  Data object that represents a 3D object.
  */
class rt3DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! Different types of ray cast functions
  enum RayCastFunction {
    RCF_COMPOSITE,
    RCF_ISOSURFACE,
    RCF_MIP
  };

  //! Constructor
  rt3DVolumeDataObject();

  //! Destructor
  ~rt3DVolumeDataObject();

  //! Get a handle to the image data object.
  /*!
  This function provides a way to access and to modify the image data object.
  Do not delete the handle returned by this object. The creation and deletion is handled by the Data Object.
  @return A handle to the image data object.
  */
  vtkImageData* getImageData();


  vtkImageData* getUShortData();
  vtkTransform* getTransform();
  vtkPiecewiseFunction* getPieceFunc();
  vtkColorTransferFunction* getColorTransFunc();
  vtkVolumeProperty* getVolumeProperty();

  RayCastFunction getRayCastType() { return m_rayCastFunction; }
  vtkVolumeRayCastFunction* getRayCastFunction();

  //! Set the new image data.
  /*!
  Setting the new image data will cause a lot of re-setting of parameters and options. Volume properites and transfer functions will be modified. The image data will be copied over to this object. This finction emits a newImageData() signal if successful.
  @param temp The ImageData to be copied.
  @return true if the data was copied correctly.
  */
  bool copyNewImageData(vtkImageData* temp);

  //! Set a new position transform for the image data.
  /*!
    @param temp The new transform to use.
    @return true if the new transform was copied.
    */
  bool copyNewTransform(vtkTransform* temp);

  //! Check if the volume should be rendered.
  bool getRenderRayTraceVolume() { return m_optionsWidget.groupRayCastVolume->isChecked(); }

  bool getAxial3D() { return m_optionsWidget.checkAxial3D->isChecked(); }
  bool getSagittal3D() { return m_optionsWidget.checkSagittal3D->isChecked(); }
  bool getCoronal3D() { return m_optionsWidget.checkCoronal3D->isChecked(); }

  bool isDataValid() { return m_imgDataValid; }

  //! Translate the data object
  void translateData(double x, double y, double z);
  void scaleData(double x, double y, double z);
  void setDirectionCosinesXY(float* dirCos);

  int getInterpolation() { return m_interpolationType; }

  void update();

  int getVisibleComponent() { return m_visibleComponent; }

  //! Get the window part of the window level.
  int getWindow() { return m_window; }
  //! Get the level part of the window level.
  int getLevel() { return m_level; }

 public slots:
  void surfaceFunctionChanged();

  void newObjectCreated(int id);
  void oldObjectRemoved(int id);

  //! A new color transfer function was chosen through the GUI
  void colorTransferChangedGUI(QString id);
  //! The color transfer function has changed
  void colorTransferChanged(int);

  //! A new piecewise function has been chosen from the GUI
  void piecewiseChangedGUI(QString id);
  //! The piecewise function has changed
  void piecewiseChanged(int);

  //! The user has changed the intrpolation type from the GUI
  void interpolationChanged(int interp);

  void setVisibleComponent(int c);
  void nextVisibleComponent();

  void cineLoop(bool);

  void showWindowLevel();

  //! The window level has been changed by WL dialog.
  void wlChanged(int w, int l);

 signals:
  void newImageData();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  vtkImageData* m_imgData;
  vtkTransform* m_dataTransform;
  vtkPiecewiseFunction* m_pieceFunc;
  vtkColorTransferFunction* m_colorTransFunc;
  vtkVolumeProperty* m_volumeProperty;
  vtkImageShiftScale * m_imgUShortCast;

  vtkPiecewiseFunction* m_pieceFuncDefault;
  vtkColorTransferFunction* m_colorTransFuncDefault;

  //! The ID of the external piecewise function used.
  int m_piecewiseFuncID;
  //! The ID of the external color function used.
  int m_colorFuncID;

  double m_window;
  double m_level;

  vtkSmartPointer<vtkImageExtractComponents> m_subImg;
  int m_visibleComponent;
  QTimer *m_cineFrame;

  bool m_imgDataValid;
  //! The type of interpolation that will be used by the cut planes and the volume reslice.
  /*! The three types possible are: Nearest Neighbour = 0, Linear = 1, Cubic = 2
    */
  int m_interpolationType;

  // Ray Cast Functions
  RayCastFunction m_rayCastFunction;
  vtkVolumeRayCastCompositeFunction* m_compositeFunc;
  vtkVolumeRayCastIsosurfaceFunction* m_isosurfaceFunc;
  vtkVolumeRayCastMIPFunction* m_MIPFunc;

  // UI objects
  Ui::volume3DOptions m_optionsWidget;

  rtWindowLevelDialog *m_wlDialog;

};

#endif 
