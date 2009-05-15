#include "rt3dVolumeDataObject.h"


//! Constructor
rt3DVolumeDataObject::rt3DVolumeDataObject() {
  setObjectType(rtConstants::OT_3DObject);

  m_rayCastFunction=RCF_COMPOSITE;
  m_imgDataValid = false;

  m_imgUShortCast = vtkImageShiftScale::New();
  m_imgData = vtkImageData::New();
  m_dataTransform = vtkTransform::New();
  m_pieceFunc = vtkPiecewiseFunction::New();
  m_colorTransFunc = vtkColorTransferFunction::New();
  m_volumeProperty = vtkVolumeProperty::New();

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);
  m_imgUShortCast->SetInput(m_imgData);
  m_imgUShortCast->SetOutputScalarTypeToUnsignedShort();

  m_compositeFunc = vtkVolumeRayCastCompositeFunction::New();
  m_isosurfaceFunc = vtkVolumeRayCastIsosurfaceFunction::New();
  m_MIPFunc = vtkVolumeRayCastMIPFunction::New();

  setupGUI();
}

//! Destructor
rt3DVolumeDataObject::~rt3DVolumeDataObject() {
  cleanupGUI();

  m_imgUShortCast->Delete();
  m_imgData->Delete();
  m_dataTransform->Delete();
  m_pieceFunc->Delete();
  m_colorTransFunc->Delete();
  m_volumeProperty->Delete();

  m_compositeFunc->Delete();
  m_isosurfaceFunc->Delete();
  m_MIPFunc->Delete();
}


//! Take the info from the GUI (When the apply button is pressed)
void rt3DVolumeDataObject::apply() {

}

void rt3DVolumeDataObject::surfaceFunctionChanged() {
  if(m_optionsWidget.radioComposite->isChecked()) {
    m_rayCastFunction=RCF_COMPOSITE;
    Modified();
  } else if (m_optionsWidget.radioIsosurface->isChecked()) {
    m_rayCastFunction=RCF_ISOSURFACE;
    Modified();
  } else if (m_optionsWidget.radioMIP->isChecked()) {
    m_rayCastFunction=RCF_MIP;
    Modified();
  }
}

//! Send the info to the GUI
void rt3DVolumeDataObject::update() {
  switch(m_rayCastFunction) {
    case (RCF_COMPOSITE):
    m_optionsWidget.radioComposite->setChecked(true);
    m_optionsWidget.radioIsosurface->setChecked(false);
    m_optionsWidget.radioMIP->setChecked(false);
    break;
    case (RCF_ISOSURFACE):
    m_optionsWidget.radioComposite->setChecked(false);
    m_optionsWidget.radioIsosurface->setChecked(true);
    m_optionsWidget.radioMIP->setChecked(false);
    break;
    case (RCF_MIP):
    m_optionsWidget.radioComposite->setChecked(false);
    m_optionsWidget.radioIsosurface->setChecked(false);
    m_optionsWidget.radioMIP->setChecked(true);
    break;
  }
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

//! Get the image data but cast to Unsigned Short
/*!
  The mapper requires either unsigned short or unsigned char to work properly. This function makes it easier to implement that mapper.
  */
vtkImageData* rt3DVolumeDataObject::getUShortData() {
  return m_imgUShortCast->GetOutput();
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
vtkVolumeProperty* rt3DVolumeDataObject::getVolumeProperty() {
  return m_volumeProperty;
}


vtkVolumeRayCastFunction* rt3DVolumeDataObject::getRayCastFunction() {
  vtkVolumeRayCastFunction* temp = NULL;

  switch(m_rayCastFunction) {
    case (RCF_COMPOSITE):
    temp=m_compositeFunc;
    break;
    case (RCF_ISOSURFACE):
    temp=m_isosurfaceFunc;
    break;
    case (RCF_MIP):
    temp=m_MIPFunc;
    break;
  }
  return temp;
}

//! Set the new image data.
/*!
  Setting the new image data will cause a lot of re-setting of parameters and options. Volume properites and transfer functions will be modified. The image data will be copied over to this object.
  @param temp The ImageData to be copied.
  @return true if the data was copied correctly.
  */
bool rt3DVolumeDataObject::copyNewImageData(vtkImageData* temp) {
  if (!temp) return false;

  double rangeI[2], rangeP[2];


  m_imgData->DeepCopy(temp);
  m_imgData->GetScalarRange(rangeI);

  m_imgUShortCast->SetShift(-rangeI[0]);
  m_imgUShortCast->Update();
  m_imgUShortCast->GetOutput()->GetScalarRange(rangeP);

  //std::cout << "Range: " << rangeP[0] << " " << rangeP[1] << std::endl;

  m_pieceFunc->RemoveAllPoints();
  m_pieceFunc->AddPoint(rangeP[0], 0.0);
  m_pieceFunc->AddPoint(rangeP[1], 1.0);

  m_colorTransFunc->RemoveAllPoints();
  m_colorTransFunc->AddRGBPoint(rangeP[0], 0.0, 0.0, 0.0);
  m_colorTransFunc->AddRGBPoint(rangeP[1], 255.0, 15.0, 15.0);
  m_imgDataValid = true;
  Modified();

  return true;
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
  m_optionsWidget.setupUi(getBaseWidget());

  m_optionsWidget.comboPieceFunc->addItem("Default");
  m_optionsWidget.comboCTFunc->addItem("Default");

  // Global ray cast volume switch
  connect(m_optionsWidget.groupRayCastVolume, SIGNAL(toggled(bool)), this, SLOT(Modified()));

  // Surface functions.
  connect(m_optionsWidget.radioComposite, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));
  connect(m_optionsWidget.radioIsosurface, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));
  connect(m_optionsWidget.radioMIP, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));

  // 2D plane check boxes
  connect(m_optionsWidget.checkAxial3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));
  connect(m_optionsWidget.checkSagittal3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));
  connect(m_optionsWidget.checkCoronal3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));

}

//! Clean the GUI widgets.
void rt3DVolumeDataObject::cleanupGUI() {

}
