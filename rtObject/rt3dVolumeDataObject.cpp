#include "rt3dVolumeDataObject.h"

//! Constructor
rt3DVolumeDataObject::rt3DVolumeDataObject() {
  setObjectType(rtConstants::OT_3DObject);

  m_imgData = vtkImageData::New();
  m_dataTransform = vtkTransform::New();
  m_pieceFunc = vtkPiecewiseFunction::New();
  m_colorTransFunc = vtkColorTransferFunction::New();
  m_volumeProperty = vtkProperty::New();

  setupGUI();
}

//! Destructor
rt3DVolumeDataObject::~rt3DVolumeDataObject() {
  cleanupGUI();

  m_imgData->Delete();
  m_dataTransform->Delete();
  m_pieceFunc->Delete();
  m_colorTransFunc->Delete();
  m_volumeProperty->Delete();
}


//! Take the info from the GUI
void rt3DVolumeDataObject::apply() {

}

//! Send the info to the GUI
void rt3DVolumeDataObject::update() {

}

//! Get a handle to the image data object.
/*!
  This function provides a way to access and to modify the image data object.
  Do not delete the handle returned by this object. The creation and deletion is handled by the Data Object.
  @return A handle to the image data object.
  */
vtkImageData* rt3DVolumeDataObject::getImageData() {
  return m_imgData;
}

//! Get a handle to the transform.
/*!
  This function provides a way to access and to modify the transform.
  Do not delete the handle returned by this object. The creation and deletion is handled by the Data Object.
  @return A handle to the image data object.
  */
vtkTransform* rt3DVolumeDataObject::getTransform() {
  return m_dataTransform;
}

//! Get the piecewise function
vtkPiecewiseFunction* rt3DVolumeDataObject::getPieceFunc() {
  return m_pieceFunc;
}

//! Get the color transfer function
vtkColorTransferFunction* rt3DVolumeDataObject::getColorTransFunc() {
  return m_colorTransFunc;
}

//! Get volume property
vtkProperty* rt3DVolumeDataObject::getVolumeProperty() {
  return m_volumeProperty;
}

//! Translate the data object
void rt3DVolumeDataObject::translateData(double x, double y, double z) {
  m_dataTransform->Translate(x,y,z);
}

void rt3DVolumeDataObject::scaleData(double x, double y, double z) {
  m_dataTransform->Scale(x,y,z);
}

//! Flip the direction of the X-axis
void rt3DVolumeDataObject::flipX() {
  m_dataTransform->Scale(-1,1,1);
}

//! Flip the direction of the Y-axis
void rt3DVolumeDataObject::flipY() {
  m_dataTransform->Scale(1,-1,1);
}

//! Flip the direction of the Z-axis
void rt3DVolumeDataObject::flipZ() {
  m_dataTransform->Scale(1,1,-1);
}


//! Set the GUI widgets.
void rt3DVolumeDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt3DVolumeDataObject::cleanupGUI() {

}
