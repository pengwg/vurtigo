#ifndef RT_3D_VOLUME_DATA_OBJECT_H
#define RT_3D_VOLUME_DATA_OBJECT_H

#include "rtDataObject.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"

//! 3D Volume Data Object
/*!
  Data object that represents a 3D object.
  */
class rt3DVolumeDataObject : public rtDataObject
{
Q_OBJECT

public:
  rt3DVolumeDataObject();
  ~rt3DVolumeDataObject();

  vtkImageData* getImageData();
  vtkTransform* getTransform();
  vtkPiecewiseFunction* getPieceFunc();
  vtkColorTransferFunction* getColorTransFunc();
  vtkVolumeProperty* getVolumeProperty();

  void translateData(double x, double y, double z);
  void scaleData(double x, double y, double z);
  void flipX();
  void flipY();
  void flipZ();

  void apply();
  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  vtkImageData* m_imgData;
  vtkTransform* m_dataTransform;
  vtkPiecewiseFunction* m_pieceFunc;
  vtkColorTransferFunction* m_colorTransFunc;
  vtkVolumeProperty* m_volumeProperty;
};

#endif 
