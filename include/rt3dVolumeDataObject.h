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
#include "DICOMCommonData.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkImageShiftScale.h"
#include "vtkImageExtractComponents.h"

#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastIsosurfaceFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkImageClip.h>

#include <QTimer>

//! 3D Volume Data Object
/*!
  Data object that represents a 3D object. This object is also capable of playing back 4D (3D+time) cine volumes.
  */
class rt3DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! Different types of ray cast functions
  /*!
    Composite - Render the voxles based on the tracing rays to the first visible voxel and rendering that.
    Isosurface - Create a surface based on a threshold value.
    MIP - Maximum intensity projection also based on tracing rays through the volume and rendering the voxel of highest intensity.
    */
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
  \return A handle to the image data object.
  */
  vtkImageData* getImageData();

  //! Get the image data but cast to Unsigned Short
  /*!
  The mapper requires either unsigned short or unsigned char to work properly. This function makes it easier to implement that mapper.
  \return A pointer to the unsigned short version of the data.
  */
  vtkImageData* getUShortData();


  //! Get a handle to the transform for the 3D volume
  /*!
  This function provides a way to access and to modify the transform.
  Do not delete the handle returned by this object. The creation and deletion is handled by the Data Object.
  \return A handle to the image data object.
  */
  vtkTransform* getTransform();

  //! Get the piecewise function for the 3D volume
  vtkPiecewiseFunction* getPieceFunc();

  //! Get the current color transfer function
  vtkColorTransferFunction* getColorTransFunc();

  //! Get volume property
  vtkVolumeProperty* getVolumeProperty();

  //! Get the type of ray cast function
  RayCastFunction getRayCastType() { return m_rayCastFunction; }

  //! Get the ray cast function
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

  //! Set the trigger delay list.
  inline void copyTriggerDelayList(QList<double> *trigDelay) { if(!trigDelay) return; m_triggerList = (*trigDelay); }

  //! Set the DICOM file information.
  inline void copyDicomCommonData(DICOMCommonData* commonData) { if(!commonData) return; m_commonData.deepCopy(commonData); }

  //! Check if the volume should be rendered.
  bool getRenderRayTraceVolume() { return m_optionsWidget.groupRayCastVolume->isChecked(); }

  //! Check if the 3D axial slice is being shown
  bool getAxial3D() { return m_optionsWidget.checkAxial3D->isChecked(); }
  //! Check if the 3D sagittal slice is being shown
  bool getSagittal3D() { return m_optionsWidget.checkSagittal3D->isChecked(); }
  //! Check if the 3D coronal slice is being shown
  bool getCoronal3D() { return m_optionsWidget.checkCoronal3D->isChecked(); }

  //! Check if valid data exists in the data object.
  /*!
    When the data obejct is created the data is not valid. When new image data is copied into the data object then it becomes valid.
    */
  bool isDataValid() { return m_imgDataValid; }

  //! Translate the data object
  void translateData(double x, double y, double z);
  //! Scale the data object
  void scaleData(double x, double y, double z);
  //! Set the direction cosines for X and Y. The Z will be calculated from there. This function must be called FIRST.
  void setDirectionCosinesXY(float* dirCos);

  //! Get the type of interpolation used.
  /*!
    The three types possible are: Nearest Neighbour = 0, Linear = 1, Cubic = 2
    \return The interpolation value as an integer.
    */
  int getInterpolation() { return m_interpolationType; }

  //! Send the info to the GUI
  void update();

  int getVisibleComponent() { return m_visibleComponent; }

  //! Get the window part of the window level.
  int getWindow() { return m_window; }
  //! Get the level part of the window level.
  int getLevel() { return m_level; }

  //! Get the annotation status.
  /*!
    \return true if the annotation check box is checked. false otherwise.
    */
  inline bool getDoAnnotate() { return m_optionsWidget.annotateCheckBox->isChecked(); }

  //! Get the annotation string.
  /*!
    \return The annotation string to be displayed.
    */
  inline QString getAnnotation() { return m_annotation; }

  //! Get the transform matrix for a particular plane.
  vtkTransform* getTransformForPlane(int id) {
    if (id < 0 || id > 2) return NULL;
    return m_planeTransform[id];
  }

  //! Set the transform for one of the three planes.
  void setTransformForPlane(int id, vtkTransform* t) {
    if (id < 0 || id > 2) return;
    return m_planeTransform[id]->DeepCopy(t);
  }

 public slots:
  //! Slot called when the user changes the surface function.
  void surfaceFunctionChanged();

  //! Slot is called when Vurtigo creates a new object.
  void newObjectCreated(int id);

  //! Slot is called when Vurtigo deletes an existing object.
  void oldObjectRemoved(int id);

  //! A new color transfer function was chosen through the GUI
  void colorTransferChangedGUI(QString id);
  //! The color transfer function has changed
  void colorTransferChanged(int);

  //! A new piecewise function has been chosen from the GUI
  void piecewiseChangedGUI(QString id);
  //! The piecewise function has changed from the GUI
  void piecewiseChanged(int);

  //! The user has changed the intrpolation type from the GUI
  /*!
    The three types possible are: Nearest Neighbour = 0, Linear = 1, Cubic = 2
    \param interp The interpolation value.
    */
  void interpolationChanged(int interp);

  void setVisibleComponent(int c);
  void nextVisibleComponent();

  void isoValueChanged(int v);

  void cineLoop(bool);

  //! Show the window level dialog
  void showWindowLevel();

  //! The window level has been changed by WL dialog.
  void wlChanged(int w, int l);

  //! The button was pressed to reset the plane to the axial position
  void axialResetSlot();
  //! The button was pressed to reset the plane to the sagittal position
  void sagittalResetSlot();
  //! The button was pressed to reset the plane to the coronal position
  void coronalResetSlot();

  //! Update the info in the GUI text box for this volume object.
  void updateInfoText();

  //! Turn the cropping feature on or off
  void cropStatusChanged(bool);

  //! The slider for the min x value has changed
  void xminSliderChanged(int);
  //! The slider for the max x value has changed
  void xmaxSliderChanged(int);
  //! The slider for the min y value has changed
  void yminSliderChanged(int);
  //! The slider for the max y value has changed
  void ymaxSliderChanged(int);
  //! The slider for the min z value has changed
  void zminSliderChanged(int);
  //! The slider for the max z value has changed
  void zmaxSliderChanged(int);

  //! Create a new piecewisefunction and select it for the rendering
  void createNewPWF();
  //! Create a new color transfer function and select that for rendering.
  void createNewCTF();

  //! Determine if the ray cast volume is pickable
  inline bool isVolumePickable() { return m_optionsWidget.volumePickableCheckBox->isChecked(); }

  //! Set the volume as pickable or not.
  inline void setVolumePickable(bool pick) {
    if (pick != m_optionsWidget.volumePickableCheckBox->isChecked()) {
      m_optionsWidget.volumePickableCheckBox->setChecked(pick);
      Modified();
    }
  }

 signals:
  //! The volume has received new image data
  void newImageData();

  void axialResetSignal();
  void sagittalResetSignal();
  void coronalResetSignal();

 protected:
  /////////////
  // Functions
  /////////////
  //! Set the GUI widgets.
  void setupGUI();
  //! Cleanup the GUI widgets.
  void cleanupGUI();

  ////////////////
  // Variables
  ///////////////
  vtkImageData* m_imgData;
  vtkImageClip* m_imgClip;
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

  //! Meta-data is filled in if it is read from DICOM files.
  DICOMCommonData m_commonData;

  //! The window value
  double m_window;
  //! The level value
  double m_level;

  //! The annotation for this volume.
  QString m_annotation;

  QList<double> m_triggerList;
  vtkSmartPointer<vtkImageExtractComponents> m_subImg;
  int m_visibleComponent;
  QTimer *m_cineFrame;

  //! Flag to determine if the image data has been initialized.
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
  //! The widget for the options for this object
  Ui::volume3DOptions m_optionsWidget;

  //! The dialog object where the user changes window and level paramters.
  rtWindowLevelDialog *m_wlDialog;

  //! Positions for the three planes
  vtkTransform* m_planeTransform[3];


};

#endif 
