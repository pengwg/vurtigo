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
  */
bool rt2DSliceDataObject::copyImageData2D(vtkImageData* img) {
  if (!img) return false;

  double rangeI[2];

  m_imgData->DeepCopy(img);
  m_imgData->GetScalarRange(rangeI);

  m_imgUCharCast->SetShift(-rangeI[0]);

  m_imgDataValid = true;
  Modified();

  return true;
}

//! Set the parameters and the image.
/*!
  If the parameters change this operation becomes more expensive.
  @param FOV Field of view.
  @param imgSize The size of one side of the square image in pixels.
  @param numChan The number of channels.
  @param imgVectorPtr A pointer to a vector containing all of the image information. The vector is copied so it can safely be deleted once this function returns.
  */
bool rt2DSliceDataObject::setImageParameters(int FOV, int imgSize, int numChan, std::vector<unsigned char>* imgVectorPtr) {
  if ( !imgVectorPtr ) return false;
  if ( FOV<=0 || imgSize<=0 || numChan<=0 ) return false;
  if ( imgSize*imgSize*numChan != imgVectorPtr->size() ) return false;

  double spacing;
  spacing = (double)((double)FOV/(double)imgSize);

  if (m_FOV != FOV || m_imgSize!=imgSize || m_numChan!=numChan) {

    m_FOV = FOV; m_imgSize = imgSize; m_numChan = numChan;

    m_imgData->Initialize();
    m_imgData->SetScalarTypeToUnsignedChar();
    m_imgData->SetSpacing(spacing, spacing, 1.0);
    m_imgData->SetOrigin(0.0,0.0,0.0);
    m_imgData->SetDimensions(imgSize,imgSize,1);
    m_imgData->SetNumberOfScalarComponents(numChan);
    m_imgData->SetWholeExtent(-FOV/2.0f,FOV/2.0f,-FOV/2.0f,FOV/2.0f,0,1);
    m_imgData->AllocateScalars();
  }

  // Copy the data...
  int pos = 0;
  for (int ix1=0; ix1<imgSize; ix1++) {
    for (int ix2=0; ix2<imgSize; ix2++) {
      for (int ix3=0; ix3<numChan; ix3++) {
        m_imgData->SetScalarComponentFromFloat(ix1, ix2, 0, ix3, (float)(*imgVectorPtr)[pos]);
        pos++;
      }
    }
  }

  m_imgData->Modified();

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
