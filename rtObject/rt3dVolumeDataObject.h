#ifndef RT_3D_VOLUME_DATA_OBJECT_H
#define RT_3D_VOLUME_DATA_OBJECT_H

#include "rtDataObject.h"
#include "ui_volume3DOptions.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkImageShiftScale.h"

#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"

//! 3D Volume Data Object
/*!
  Data object that represents a 3D object.
  */
class rt3DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:

  enum RayCastFunction {
    RCF_COMPOSITE,
    RCF_ISOSURFACE,
    RCF_MIP
  };

  rt3DVolumeDataObject();
  ~rt3DVolumeDataObject();

  vtkImageData* getImageData();
  vtkImageData* getUShortData();
  vtkTransform* getTransform();
  vtkPiecewiseFunction* getPieceFunc();
  vtkColorTransferFunction* getColorTransFunc();
  vtkVolumeProperty* getVolumeProperty();

  RayCastFunction getRayCastType() { return m_rayCastFunction; }
  vtkVolumeRayCastFunction* getRayCastFunction();

  bool copyNewImageData(vtkImageData* temp);

  //! Check if the volume should be rendered.
  bool getRenderRayTraceVolume() { return m_optionsWidget.groupRayCastVolume->isChecked(); }

  bool getAxial3D() { return m_optionsWidget.checkAxial3D->isChecked(); }
  bool getSagittal3D() { return m_optionsWidget.checkSagittal3D->isChecked(); }
  bool getCoronal3D() { return m_optionsWidget.checkCoronal3D->isChecked(); }

  bool isDataValid() { return m_imgDataValid; }

  void translateData(double x, double y, double z);
  void scaleData(double x, double y, double z);
  void flipX();
  void flipY();
  void flipZ();

  void apply();
  void update();

 public slots:
  void surfaceFunctionChanged();

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

  bool m_imgDataValid;

  // Ray Cast Functions
  RayCastFunction m_rayCastFunction;
  vtkVolumeRayCastCompositeFunction* m_compositeFunc;
  vtkVolumeRayCastIsosurfaceFunction* m_isosurfaceFunc;
  vtkVolumeRayCastMIPFunction* m_MIPFunc;

  // UI objects
  Ui::volume3DOptions m_optionsWidget;

};

#endif 
