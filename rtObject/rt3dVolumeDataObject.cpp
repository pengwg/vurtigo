#include "rt3dVolumeDataObject.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "objTypes.h"

#include "rtColorFuncDataObject.h"
#include "rtColorFuncRenderObject.h"

//! Constructor
rt3DVolumeDataObject::rt3DVolumeDataObject() {
  setObjectType(rtConstants::OT_3DObject);

  m_rayCastFunction=RCF_COMPOSITE;
  m_imgDataValid = false;

  m_imgUShortCast = vtkImageShiftScale::New();
  m_imgData = vtkImageData::New();
  m_dataTransform = vtkTransform::New();
  m_pieceFuncDefault = vtkPiecewiseFunction::New();
  m_colorTransFuncDefault = vtkColorTransferFunction::New();
  m_volumeProperty = vtkVolumeProperty::New();

  // The the functions as default for now.
  m_pieceFunc = m_pieceFuncDefault;
  m_colorTransFunc = m_colorTransFuncDefault;

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);
  m_imgUShortCast->SetInput(m_imgData);
  m_imgUShortCast->SetOutputScalarTypeToUnsignedShort();

  m_compositeFunc = vtkVolumeRayCastCompositeFunction::New();
  m_isosurfaceFunc = vtkVolumeRayCastIsosurfaceFunction::New();
  m_MIPFunc = vtkVolumeRayCastMIPFunction::New();

  // Setup the interpolation
  m_interpolationType = 1; // Linear interp

  setupGUI();
}

//! Destructor
rt3DVolumeDataObject::~rt3DVolumeDataObject() {
  cleanupGUI();

  m_imgUShortCast->Delete();
  m_imgData->Delete();
  m_dataTransform->Delete();
  m_pieceFuncDefault->Delete();
  m_colorTransFuncDefault->Delete();
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

//! Get the current piecewise function
vtkPiecewiseFunction* rt3DVolumeDataObject::getPieceFunc() {
  return m_pieceFunc;
}

//! Get the current color transfer function
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

  m_pieceFuncDefault->RemoveAllPoints();
  m_pieceFuncDefault->AddPoint(rangeP[0], 0.0);
  m_pieceFuncDefault->AddPoint(rangeP[1], 1.0);

  m_colorTransFuncDefault->RemoveAllPoints();
  m_colorTransFuncDefault->AddRGBPoint(rangeP[0], 0.0, 0.0, 0.0);
  m_colorTransFuncDefault->AddRGBPoint(rangeP[1], 1.0, 0.5, 0.5);

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);

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

//! Set the direction cosines for X and Y. The Z will be calculated from there. This function must be called FIRST.
void rt3DVolumeDataObject::setDirectionCosinesXY(float* dirCos) {
  vtkMatrix4x4* mat = vtkMatrix4x4::New();

  mat->Identity();
  mat->SetElement(0, 0, dirCos[0]);
  mat->SetElement(1, 0, dirCos[1]);
  mat->SetElement(2, 0, dirCos[2]);
  mat->SetElement(0, 1, dirCos[3]);
  mat->SetElement(1, 1, dirCos[4]);
  mat->SetElement(2, 1, dirCos[5]);

    // Find the third vector by doing the cross product of the first two. (USE the flipped data!!)
  mat->SetElement(0, 2, mat->GetElement(1,0)*mat->GetElement(2,1) - mat->GetElement(2,0)*mat->GetElement(1,1));
  mat->SetElement(1, 2, mat->GetElement(2,0)*mat->GetElement(0,1) - mat->GetElement(0,0)*mat->GetElement(2,1));
  mat->SetElement(2, 2, mat->GetElement(0,0)*mat->GetElement(1,1) - mat->GetElement(1,0)*mat->GetElement(0,1));

  //mat->Print(std::cout);
  mat->Invert();
  //mat->Print(std::cout);
  m_dataTransform->SetMatrix(mat);
  mat->Delete();
}

//! Flip the direction of the X-axis
void rt3DVolumeDataObject::flipX() {
  m_dataTransform->Scale(-1,1,1);
  Modified();
}

//! Flip the direction of the Y-axis
void rt3DVolumeDataObject::flipY() {
  m_dataTransform->Scale(1,-1,1);
  Modified();
}

//! Flip the direction of the Z-axis
void rt3DVolumeDataObject::flipZ() {
  m_dataTransform->Scale(1,1,-1);
  Modified();
}


//! Set the GUI widgets.
void rt3DVolumeDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  // Interpolation Type.
  connect(m_optionsWidget.interpolateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(interpolationChanged(int)));

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

  // Watch for new objects to update the lists.
  connect(&rtObjectManager::instance(), SIGNAL(objectCreated(int)), this, SLOT(newObjectCreated(int)));
  connect(&rtObjectManager::instance(), SIGNAL(objectCreated(int)), this, SLOT(oldObjectRemoved(int)));

  // The combo boxes for the CTF and PWF.
  connect(m_optionsWidget.comboCTFunc, SIGNAL(currentIndexChanged(QString)), this, SLOT(colorTransferChanged(QString)));
  connect(m_optionsWidget.comboPieceFunc, SIGNAL(currentIndexChanged(QString)), this, SLOT(piecewiseChanged(QString)));

  // The flip axis signals.
  connect(m_optionsWidget.flipXCheck, SIGNAL(stateChanged(int)), this, SLOT(flipX()));
  connect(m_optionsWidget.flipYCheck, SIGNAL(stateChanged(int)), this, SLOT(flipY()));
  connect(m_optionsWidget.flipZCheck, SIGNAL(stateChanged(int)), this, SLOT(flipZ()));

  m_optionsWidget.comboPieceFunc->clear();
  m_optionsWidget.comboPieceFunc->addItem("Default");
  QList<int> pieceFuncs = rtObjectManager::instance().getObjectsOfType(rtConstants::OT_vtkPiecewiseFunction);
  for (int ix1=0; ix1<pieceFuncs.count() ; ix1++) {
    m_optionsWidget.comboPieceFunc->addItem(QString::number(pieceFuncs.at(ix1)));
  }

  m_optionsWidget.comboCTFunc->clear();
  m_optionsWidget.comboCTFunc->addItem("Default");
  QList<int> colorFuncs = rtObjectManager::instance().getObjectsOfType(rtConstants::OT_vtkColorTransferFunction);
  for (int ix1=0; ix1<colorFuncs.count() ; ix1++) {
    m_optionsWidget.comboCTFunc->addItem(QString::number(colorFuncs.at(ix1)));
  }

  m_optionsWidget.interpolateComboBox->setCurrentIndex(m_interpolationType);

}

//! Slot is called when Vurtigo creates a new object.
void rt3DVolumeDataObject::newObjectCreated(int id) {
  rtRenderObject * temp = rtObjectManager::instance().getObjectWithID(id);
  if (temp) {
    if (temp->getObjectType()==rtConstants::OT_vtkPiecewiseFunction) {
      // New Piecewise Function.
      // Do the reset... It may be faster to just add the object but this is more robust.
      m_optionsWidget.comboPieceFunc->clear();
      m_optionsWidget.comboPieceFunc->addItem("Default");
      QList<int> pieceFuncs = rtObjectManager::instance().getObjectsOfType(rtConstants::OT_vtkPiecewiseFunction);
      for (int ix1=0; ix1<pieceFuncs.count() ; ix1++) {
        m_optionsWidget.comboPieceFunc->addItem(QString::number(pieceFuncs.at(ix1)));
      }
    } else if (temp->getObjectType()==rtConstants::OT_vtkColorTransferFunction) {
      // New Color Function.
      m_optionsWidget.comboCTFunc->clear();
      m_optionsWidget.comboCTFunc->addItem("Default");
      QList<int> colorFuncs = rtObjectManager::instance().getObjectsOfType(rtConstants::OT_vtkColorTransferFunction);
      for (int ix1=0; ix1<colorFuncs.count() ; ix1++) {
        m_optionsWidget.comboCTFunc->addItem(QString::number(colorFuncs.at(ix1)));
      }
    }

  }
}

void rt3DVolumeDataObject::oldObjectRemoved(int id) {

}

//! A new color transfer function was chosen through the GUI
void rt3DVolumeDataObject::colorTransferChanged(QString id) {
  bool ok;
  int idInt;

  if (id == "Default") {
    // Setup the default CTF.
    m_colorTransFunc = m_colorTransFuncDefault;
  } else {

    idInt = id.toInt(&ok);
    if (ok) {
      rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtObjectManager::instance().getObjectWithID(idInt)->getDataObject());
      m_colorTransFunc = func->getColorFunction();
    }
  }

  m_volumeProperty->SetColor(m_colorTransFunc);
  Modified();
}

//! A new piecewise function was chosen through the GUI
void rt3DVolumeDataObject::piecewiseChanged(QString id) {
  bool ok;
  int idInt;

  if (id == "Default") {
    // Setup the default CTF.
    m_pieceFunc = m_pieceFuncDefault;
  } else {

    idInt = id.toInt(&ok);
    if (ok) {
      rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtObjectManager::instance().getObjectWithID(idInt)->getDataObject());
      m_colorTransFunc = func->getColorFunction();
    }
  }
  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  Modified();
}


//! Clean the GUI widgets.
void rt3DVolumeDataObject::cleanupGUI() {

}

//! Change how the interpolation is done.
void rt3DVolumeDataObject::interpolationChanged(int interp) {
  m_interpolationType = interp;
  Modified();
}

