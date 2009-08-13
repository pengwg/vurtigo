#include "rt2dSliceDataObject.h"

//! Constructor
rt2DSliceDataObject::rt2DSliceDataObject() {
  setObjectType(rtConstants::OT_2DObject);
  setupGUI();

  m_imgDataValid = false;
  m_FOV = 0;
  m_imgSize = 0;
  m_numChan = 0;

  m_imgData = vtkImageData::New();
  m_trans = vtkTransform::New();
  m_imgUCharCast = vtkImageShiftScale::New();

  m_imgUCharCast->SetInput(m_imgData);
  m_imgUCharCast->SetOutputScalarTypeToUnsignedChar();
}

//! Destructor
rt2DSliceDataObject::~rt2DSliceDataObject() {
  cleanupGUI();

  if (m_imgData) m_imgData->Delete();
  if (m_trans) m_trans->Delete();
  if (m_imgUCharCast) m_imgUCharCast->Delete();
}


//! Send the info to the GUI
void rt2DSliceDataObject::update() {
  if(!m_trans) return;

  // If we have manual prescription then we do not want to update the GUI.
  if(m_optionsWidget.prescribeGroupBox->isChecked()) return;

  double translate[3];
  m_trans->GetPosition(translate);
  m_optionsWidget.xDoubleSpinBox->setValue(translate[0]);
  m_optionsWidget.yDoubleSpinBox->setValue(translate[1]);
  m_optionsWidget.zDoubleSpinBox->setValue(translate[2]);
}

//! Set the GUI widgets.
void rt2DSliceDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

 connect(m_optionsWidget.spinLeftButton, SIGNAL(clicked()), this, SLOT(spinLeft()));
 connect(m_optionsWidget.spinRightButton, SIGNAL(clicked()), this, SLOT(spinRight()));

 connect(m_optionsWidget.rotateUpButton, SIGNAL(clicked()), this, SLOT(rotateUp()));
 connect(m_optionsWidget.rotateDownButton, SIGNAL(clicked()), this, SLOT(rotateDown()));
 connect(m_optionsWidget.rotateLeftButton, SIGNAL(clicked()), this, SLOT(rotateLeft()));
 connect(m_optionsWidget.rotateRightButton, SIGNAL(clicked()), this, SLOT(rotateRight()));

 connect(m_optionsWidget.pushForwardButton, SIGNAL(clicked()), this, SLOT(pushPlane()));
 connect(m_optionsWidget.pullBackButton, SIGNAL(clicked()), this, SLOT(pullPlane()));

 connect(m_optionsWidget.xDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(xTranslate(double)));
 connect(m_optionsWidget.yDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(yTranslate(double)));
 connect(m_optionsWidget.zDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(zTranslate(double)));
}

//! Clean the GUI widgets.
void rt2DSliceDataObject::cleanupGUI() {

}

//! Copy new data over top of this one.
/*!
  */
bool rt2DSliceDataObject::copyImageData2D(vtkImageData* img) {
  if (!img) return false;

  double rangeI[2];

  m_imgData->DeepCopy(img);
  m_imgData->GetScalarRange(rangeI);

  m_imgUCharCast->SetShift(-rangeI[0]);

  m_imgDataValid = true;
  return true;
}


//! Set the trasformation matrix
bool rt2DSliceDataObject::setTransform(float rotMatrix[9], float transMatrix[3]) {
  vtkMatrix4x4* mat = vtkMatrix4x4::New();

  for (int ix1=0; ix1<3; ix1++) {
    for (int ix2=0; ix2<3; ix2++) {
      mat->SetElement(ix1, ix2, rotMatrix[ix1*3+ix2]);
    }
  }

  m_trans->Identity();
  m_trans->SetMatrix(mat);
  m_trans->Translate(transMatrix);

  if(mat) mat->Delete();
  return true;
}

//! Set the transformation in the form of a vtkMatrix4x4
bool rt2DSliceDataObject::setVtkMatrix(vtkMatrix4x4* m) {
  if (!m) return false;

  m_trans->Identity();
  m_trans->SetMatrix(m);
  return true;
}


void rt2DSliceDataObject::spinRight() {
}

void rt2DSliceDataObject::spinLeft() {
}

void rt2DSliceDataObject::rotateUp() {
}

void rt2DSliceDataObject::rotateDown() {
}

void rt2DSliceDataObject::rotateLeft() {
}

void rt2DSliceDataObject::rotateRight() {
}

void rt2DSliceDataObject::pushPlane() {
}

void rt2DSliceDataObject::pullPlane() {
}

void rt2DSliceDataObject::xTranslate(double v) {
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  mat->SetElement(0, 3, v);
  m_trans->SetMatrix(mat);

  mat->Delete();
  Modified();
}

void rt2DSliceDataObject::yTranslate(double v) {
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  mat->SetElement(1, 3, v);
  m_trans->SetMatrix(mat);

  mat->Delete();
  Modified();
}

void rt2DSliceDataObject::zTranslate(double v) {
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  mat->SetElement(2, 3, v);
  m_trans->SetMatrix(mat);

  mat->Delete();
  Modified();
}
