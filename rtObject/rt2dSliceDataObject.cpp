#include "rt2dSliceDataObject.h"

//! Constructor
rt2DSliceDataObject::rt2DSliceDataObject() {
  setObjectType(rtConstants::OT_2DObject);
  setupGUI();

  m_imgDataValid = false;

  m_imgData = vtkImageData::New();
  m_imgReslice = vtkImageReslice::New();
  m_trans = vtkTransform::New();
  m_imgUCharCast = vtkImageShiftScale::New();

  m_imgUCharCast->SetInput(m_imgData);
  m_imgUCharCast->SetOutputScalarTypeToUnsignedChar();

  m_imgReslice->SetInput( m_imgUCharCast->GetOutput() );
  m_imgReslice->SetResliceAxes( m_trans->GetMatrix() );
}

//! Destructor
rt2DSliceDataObject::~rt2DSliceDataObject() {
  cleanupGUI();

  if (m_imgData) m_imgData->Delete();
  if (m_imgReslice) m_imgReslice->Delete();
  if (m_trans) m_trans->Delete();
  if (m_imgUCharCast) m_imgUCharCast->Delete();
}


//! Take the info from the GUI
void rt2DSliceDataObject::apply() {

}

//! Send the info to the GUI
void rt2DSliceDataObject::update() {

}

//! Set the GUI widgets.
void rt2DSliceDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt2DSliceDataObject::cleanupGUI() {

}

//! Copy new data over top of this one.
/*!
  @ todo Add this function.
  */
bool rt2DSliceDataObject::copyImageData2D(vtkImageData* img) {
  if (!img) return false;

  double rangeI[2];

  m_imgData->DeepCopy(img);
  m_imgData->GetScalarRange(rangeI);

  m_imgUCharCast->SetShift(-rangeI[0]);
  m_imgUCharCast->Update();

  m_imgDataValid = true;
  Modified();

  return true;
}
