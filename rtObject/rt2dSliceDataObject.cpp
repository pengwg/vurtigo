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
// Standard C++
#include <cmath>

// VTK
#include <vtkMath.h>

// Internal
#include "rt2dSliceDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"
#include "rtApplication.h"
#include "buildParam.h"



#include <vtkMath.h>

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
  m_lumin = vtkImageLuminance::New();

  m_window = 100;
  m_level = 50;

  m_dataCopyLock.release();
}

//! Destructor
rt2DSliceDataObject::~rt2DSliceDataObject() {
  cleanupGUI();

  if (m_imgData) m_imgData->Delete();
  if (m_trans) m_trans->Delete();
  if (m_imgUCharCast) m_imgUCharCast->Delete();
  if (m_lumin) m_lumin->Delete();

  while (!m_colorInput.empty()) {
    rt2DSliceInputColorWidget* temp = m_colorInput.takeLast();
    delete temp;
  }

  if (m_wlDialog) delete m_wlDialog;
}


//! Send the info to the GUI
void rt2DSliceDataObject::update() {
  if(!m_trans) return;

  double translate[3];
  m_trans->GetPosition(translate);
  m_optionsWidget.xDoubleSpinBox->setValue(translate[0]);
  m_optionsWidget.yDoubleSpinBox->setValue(translate[1]);
  m_optionsWidget.zDoubleSpinBox->setValue(translate[2]);
}

//! Set the GUI widgets.
void rt2DSliceDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  m_wlDialog = new rtWindowLevelDialog();

  // Add the three color inputs
  for (unsigned int ix1=0; ix1<maxNumberOfInputs(); ix1++) {
    m_colorInput.append( new rt2DSliceInputColorWidget( this->getId()) );
    m_optionsWidget.inputSelectionVertical->insertWidget(ix1, m_colorInput[ix1]);

    // Set the input id only for the current object
    if (ix1 == 0) {
      m_colorInput[ix1]->setCurrentObjectID(this->getId());
    } else {
      m_colorInput[ix1]->setCurrentObjectID(-1);
    }

    connect(m_colorInput[ix1], SIGNAL(inputColorModified(QColor)), this, SLOT(Modified()));
    connect(m_colorInput[ix1], SIGNAL(inputIdModified(int)), this, SLOT(Modified()));
  }

  connect(m_wlDialog, SIGNAL(valuesChanged(int,int)), this, SLOT(wlChanged(int,int)));

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

 connect( this, SIGNAL(copyImageData2DSignal()), this, SLOT(copyImageData2DSlot()), Qt::QueuedConnection );

 connect( m_optionsWidget.windowLevelPushButton, SIGNAL(clicked()), this, SLOT(showWindowLevel()) );
}


void rt2DSliceDataObject::cleanupGUI() {

}


bool rt2DSliceDataObject::copyImageData2D(vtkImageData* img) {
  if (!img) return false;

  if (m_dataCopyLock.tryAcquire()) {
    m_imgData->Delete();
    m_imgData = vtkImageData::New();

    m_imgData->DeepCopy(img);

    emit copyImageData2DSignal();
  } else {
    //std::cout << "Frame Copy Dropped!" << std::endl;
  }

  return true;
}

void rt2DSliceDataObject::copyImageData2DSlot() {
  double rangeI[2];

  //m_imgData->GetScalarRange(rangeI);
  m_imgUCharCast->SetOutputScalarTypeToUnsignedChar();

  m_wlDialog->setImageData( m_imgData );  

  m_imgData->GetScalarRange(rangeI);

 // xxxx EthanB keeping the m_imgUCharCast code around because the imgCast pathway in rt2dSliceRenderObject (possibly) seems to need it
  m_imgUCharCast->SetInput(m_imgData);  
  if (m_imgData->GetScalarType() == VTK_UNSIGNED_SHORT)
    {
      m_imgUCharCast->SetShift(-rangeI[0]);
      m_imgUCharCast->SetScale(255.0 / (rangeI[1] - rangeI[0])); 
    }
  else
    {
      m_imgUCharCast->SetShift(-rangeI[0]);
      m_imgUCharCast->SetScale(1.0);
    } 
  m_imgUCharCast->GetOutput()->Update();
 // this previous block of code can possibly be nixed if I can figure out the imgCast pathway

  m_dataCopyLock.release();

  m_imgDataValid = true;
}

//! Set the trasformation matrix
bool rt2DSliceDataObject::setTransform(float rotMatrix[9], float transMatrix[3], bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return false;
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
  Modified();
  return true;
}

//! Set the transformation in the form of a vtkMatrix4x4
bool rt2DSliceDataObject::setVtkMatrix(vtkMatrix4x4* m, bool asUser) {
  if (!m) return false;
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return false;
  m_trans->SetMatrix(m);
  
  Modified();
  return true;
}

bool rt2DSliceDataObject::setPlaneCenter(double center[3], bool asUser) {
  int dims[3];
  double space[3];


  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return false;

  m_imgData->GetDimensions(dims);
  m_imgData->GetSpacing(space);

  vtkMatrix4x4* mat = vtkMatrix4x4::New();

  double xOff, yOff;
  xOff = dims[0]*space[0]/2.0f;
  yOff = dims[1]*space[1]/2.0f;

  mat->DeepCopy(m_trans->GetMatrix());
  mat->SetElement(0, 3, -xOff*mat->GetElement(0, 0) - yOff*mat->GetElement(0, 1) + center[0]);
  mat->SetElement(1, 3, -xOff*mat->GetElement(1, 0) - yOff*mat->GetElement(1, 1) + center[1]);
  mat->SetElement(2, 3, -xOff*mat->GetElement(2, 0) - yOff*mat->GetElement(2, 1) + center[2]);
  m_trans->Identity();
  m_trans->SetMatrix(mat);

  if(mat) mat->Delete();
  Modified();
  return true;
}

void rt2DSliceDataObject::getPlaneCenter(double center[3]) {
  int dims[3];
  double space[3];

  m_imgData->GetDimensions(dims);
  m_imgData->GetSpacing(space);

  vtkMatrix4x4* mat = vtkMatrix4x4::New();  

  double xOff, yOff;
  xOff = dims[0]*space[0]/2.0f;
  yOff = dims[1]*space[1]/2.0f;
  
  m_trans->GetMatrix(mat);
  
  center[0] = mat->GetElement(0, 3) + xOff*mat->GetElement(0, 0) + yOff*mat->GetElement(0, 1);
  center[1] = mat->GetElement(1, 3) + xOff*mat->GetElement(1, 0) + yOff*mat->GetElement(1, 1);
  center[2] = mat->GetElement(2, 3) + xOff*mat->GetElement(2, 0) + yOff*mat->GetElement(2, 1);

  if (mat) mat->Delete();
}

bool TransformMatrixValid(vtkTransform *trans)
  {
    vtkMatrix4x4 *matrix = trans->GetMatrix();
    
    return std::isfinite(matrix->Determinant());
  }

bool rt2DSliceDataObject::setPlaneNormal(double normal[3], bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return false;

  double oldCenter[3];
  double oldNormal[3];
  double rotationAxis[3];
  double rot_angle;
  vtkTransform* tempTrans = vtkTransform::New();

  tempTrans->DeepCopy(m_trans);

  getPlaneCenter(oldCenter);
  getPlaneNormal(oldNormal);

  // Cross product to find rotation axis.
  vtkMath::Cross(oldNormal, normal, rotationAxis);
  rot_angle = ( 180/vtkMath::Pi() ) * acos(vtkMath::Dot(oldNormal, normal) / (vtkMath::Norm(oldNormal) * vtkMath::Norm(normal)));
  
   // when new and old vector are nearly the same, the calculated rotation angle can be NaN - don't try to do this rotation
  if (!std::isfinite(rot_angle))
    return false;

  tempTrans->PostMultiply();
  tempTrans->Translate(-oldCenter[0], -oldCenter[1], -oldCenter[2]);
  tempTrans->RotateWXYZ(rot_angle, rotationAxis);
  tempTrans->Translate(oldCenter[0], oldCenter[1], oldCenter[2]);
  tempTrans->PreMultiply();

 // EthanB: this test is to protect against corrupting the trans matrix 
 // I don't think it's necessary anymore (after adding the isnan check above, but I'm keeping it around for testing)
  if (!TransformMatrixValid(tempTrans))
    {
      std::cout << "WARNING: Invalid transform matrix generated in setPlaneNormal()" << endl;
      std::cout << "         Current normal vector:   <" << oldNormal[0] << " " << oldNormal[1] << " " << oldNormal[2] << ">" << endl;
      std::cout << "         Requested normal vector: <" << normal[0] << " " << normal[1] << " " << normal[2] << ">" << endl;
      std::cout << "         Rotation axis:           <" << rotationAxis[0] << " " << rotationAxis[1] << " " << rotationAxis[2] << ">" << endl;
      std::cout << "         Rotation angle:          " << rot_angle << endl;
    }

  m_trans->Identity();
  m_trans->SetMatrix(tempTrans->GetMatrix());

  if (tempTrans) tempTrans->Delete();

  Modified();
  return true;
}

void rt2DSliceDataObject::getPlaneNormal(double normal[3]) {
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);

  double zDirec[3];
  double sumSq = 0.0;
  for (int ix1=0; ix1<3; ix1++) {
    zDirec[ix1] = mat->GetElement(ix1, 2);
    sumSq += zDirec[ix1]*zDirec[ix1];
  }
  sumSq = sqrt(sumSq);

  for (int ix1 = 0; ix1 < 3; ix1++)
    normal[ix1] = zDirec[ix1] / sumSq;

  if (mat) mat->Delete();
}

bool rt2DSliceDataObject::setPlaneUp(double up[3], bool asUser)
  {
    if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return false;
    
    double oldCenter[3];
    double oldNormal[3];
    double oldUp[3];
    double rotationAxis[3];
    double rot_angle;
    getPlaneCenter(oldCenter);
    getPlaneNormal(oldNormal);
    getPlaneUp(oldUp);

   // Cross product to find rotation axis.
    vtkMath::Cross(oldUp, up, rotationAxis);
    rot_angle = ( 180/vtkMath::Pi() ) * acos(vtkMath::Dot(oldUp, up) / (vtkMath::Norm(oldUp) * vtkMath::Norm(up)));
  
   // when new and old vector are nearly the same, the calculated rotation angle can be NaN - don't try to do this rotation
    if (!std::isfinite(rot_angle))
      return false;

   // Generate matrix for composite transform 
    vtkTransform* tempTrans = vtkTransform::New();
    tempTrans->DeepCopy(m_trans);
    tempTrans->PostMultiply();
    tempTrans->Translate(-oldCenter[0], -oldCenter[1], -oldCenter[2]);
    tempTrans->RotateWXYZ(rot_angle, rotationAxis);
    tempTrans->Translate(oldCenter[0], oldCenter[1], oldCenter[2]);
    tempTrans->PreMultiply();

 // EthanB: this test is to protect against corrupting the trans matrix 
 // I don't think it's necessary anymore (after adding the isnan check above, but I'm keeping it around for testing)
    if (!TransformMatrixValid(tempTrans))
      {
        std::cout << "WARNING: Invalid transform matrix generated in setPlaneUp()" << endl;
        std::cout << "         Current up vector:    <" << oldUp[0] << " " << oldUp[1] << " " << oldUp[2] << ">" << endl;
        std::cout << "         Requested up vector:  <" << up[0] << " " << up[1] << " " << up[2] << ">" << endl;
        std::cout << "         Rotation axis:           <" << rotationAxis[0] << " " << rotationAxis[1] << " " << rotationAxis[2] << ">" << endl;
        std::cout << "         Rotation angle:          " << rot_angle << endl;
        
        return false;
      }

   // Overwrite our transform matrix
    m_trans->Identity();
    m_trans->SetMatrix(tempTrans->GetMatrix());


    if (tempTrans) 
      tempTrans->Delete();

    Modified();
    return true;
  }
  
void rt2DSliceDataObject::getPlaneUp(double up[3])
  {
    double y0[3], y1[3];
    
   // start with point at origin and point one pixel up
    y0[0] = 0; y0[1] = 0; y0[2] = 0;
    y1[0] = 0; y1[1] = 1; y1[2] = 0;
    
   // transform them both into world coordinates
    m_trans->TransformPoint(y0, y0);
    m_trans->TransformPoint(y1, y1);
    
   // find delta
    up[0] = y1[0] - y0[0];
    up[1] = y1[1] - y0[1];
    up[2] = y1[2] - y0[2];
    
    vtkMath::Normalize(up);
  }

void rt2DSliceDataObject::pushPlaneBy(double amt, bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return;

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);

  double zDirec[3];
  double sumSq = 0.0;
  for (int ix1=0; ix1<3; ix1++) {
    zDirec[ix1] = mat->GetElement(ix1, 2);
    sumSq += zDirec[ix1]*zDirec[ix1];
  }
  sumSq = sqrt(sumSq);

  double cameraDirec[3];
  double dotP=0.0f;
  rtApplication::instance().getMainWinHandle()->getCameraForward(cameraDirec);
  dotP = zDirec[0]*cameraDirec[0]+zDirec[1]*cameraDirec[1]+zDirec[2]*cameraDirec[2];
  if (dotP > 0) {
    dotP = 1.0f;
  } else if (dotP < 0) {
    dotP = -1.0f;
  }

  for (int ix1=0; ix1<3; ix1++) {
    mat->SetElement( ix1, 3, mat->GetElement(ix1, 3) + (dotP*amt*zDirec[ix1]/sumSq) );
  }
  m_trans->SetMatrix(mat);

  mat->Delete();
  Modified();
}


void rt2DSliceDataObject::spinLeftBy(double amt, bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return;

  int dims[3];
  double space[3];
  m_imgData->GetDimensions(dims);
  m_imgData->GetSpacing(space);


  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  double zDirec[3];
  for (int ix1=0; ix1<3; ix1++) {
    zDirec[ix1] = mat->GetElement(ix1, 2);
  }
  mat->Delete();

  double cameraDirec[3];
  double dotP=0.0f;
  rtApplication::instance().getMainWinHandle()->getCameraForward(cameraDirec);
  dotP = zDirec[0]*cameraDirec[0]+zDirec[1]*cameraDirec[1]+zDirec[2]*cameraDirec[2];
  if (dotP > 0) {
    dotP = 1.0f;
  } else if (dotP < 0) {
    dotP = -1.0f;
  }

  m_trans->Translate(dims[0]*space[0]/2.0f, dims[1]*space[1]/2.0f, 0.0);
  m_trans->RotateWXYZ(dotP*amt, 0.0, 0.0, 1.0);
  m_trans->Translate(-dims[0]*space[0]/2.0f, -dims[1]*space[1]/2.0f, 0.0);
  Modified();
}

void rt2DSliceDataObject::rotateUpBy(double amt, bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return;

  int dims[3];
  double space[3];
  m_imgData->GetDimensions(dims);
  m_imgData->GetSpacing(space);

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);

  double cameraDirec[3];
  double dotP1=0.0f;
  double dotP2=0.0f;
  double dotP3=0.0f;
  rtApplication::instance().getMainWinHandle()->getCameraRight(cameraDirec);
  dotP1 = mat->GetElement(0, 0)*cameraDirec[0]+mat->GetElement(1, 0)*cameraDirec[1]+mat->GetElement(2, 0)*cameraDirec[2];
  dotP2 = mat->GetElement(0, 1)*cameraDirec[0]+mat->GetElement(1, 1)*cameraDirec[1]+mat->GetElement(2, 1)*cameraDirec[2];
  dotP3 = mat->GetElement(0, 2)*cameraDirec[0]+mat->GetElement(1, 2)*cameraDirec[1]+mat->GetElement(2, 2)*cameraDirec[2];

  mat->Delete();

  if ( fabs(dotP1) > fabs(dotP2) ) {
    if (dotP1 < 0) dotP1 = -1.0f;
    else dotP1 = 1.0f;
    m_trans->Translate(dims[0]*space[0]/2.0f, dims[1]*space[1]/2.0f, 0.0);
    m_trans->RotateWXYZ(dotP1*amt, 1.0, 0.0, 0.0);
    m_trans->Translate(-dims[0]*space[0]/2.0f, -dims[1]*space[1]/2.0f, 0.0);
  } else if ( fabs(dotP1) < fabs(dotP2) ) {
    if (dotP2 < 0) dotP2 = -1.0f;
    else dotP2 = 1.0f;
    m_trans->Translate(dims[0]*space[0]/2.0f, dims[1]*space[1]/2.0f, 0.0);
    m_trans->RotateWXYZ(dotP2*amt, 0.0, 1.0, 0.0);
    m_trans->Translate(-dims[0]*space[0]/2.0f, -dims[1]*space[1]/2.0f, 0.0);
  }

  Modified();
}

void rt2DSliceDataObject::rotateLeftBy(double amt, bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return;

  int dims[3];
  double space[3];
  m_imgData->GetDimensions(dims);
  m_imgData->GetSpacing(space);

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  double cameraDirec[3];
  double dotP1=0.0f;
  double dotP2=0.0f;
  double dotP3=0.0f;
  rtApplication::instance().getMainWinHandle()->getCameraUp(cameraDirec);
  dotP1 = mat->GetElement(0, 0)*cameraDirec[0]+mat->GetElement(1, 0)*cameraDirec[1]+mat->GetElement(2, 0)*cameraDirec[2];
  dotP2 = mat->GetElement(0, 1)*cameraDirec[0]+mat->GetElement(1, 1)*cameraDirec[1]+mat->GetElement(2, 1)*cameraDirec[2];
  dotP3 = mat->GetElement(0, 2)*cameraDirec[0]+mat->GetElement(1, 2)*cameraDirec[1]+mat->GetElement(2, 2)*cameraDirec[2];

  mat->Delete();

  if ( fabs(dotP1) > fabs(dotP2) ) {
    if (dotP1 < 0) dotP1 = -1.0f;
    else dotP1 = 1.0f;
    m_trans->Translate(dims[0]*space[0]/2.0f, dims[1]*space[1]/2.0f, 0.0);
    m_trans->RotateWXYZ(dotP1*amt, 1.0, 0.0, 0.0);
    m_trans->Translate(-dims[0]*space[0]/2.0f, -dims[1]*space[1]/2.0f, 0.0);
  } else if ( fabs(dotP1) < fabs(dotP2) ) {
    if (dotP2 < 0) dotP2 = -1.0f;
    else dotP2 = 1.0f;
    m_trans->Translate(dims[0]*space[0]/2.0f, dims[1]*space[1]/2.0f, 0.0);
    m_trans->RotateWXYZ(dotP2*amt, 0.0, 1.0, 0.0);
    m_trans->Translate(-dims[0]*space[0]/2.0f, -dims[1]*space[1]/2.0f, 0.0);
  }

  Modified();
}

void rt2DSliceDataObject::translateTo(double x, double y, double z, bool asUser) {
  if (m_optionsWidget.prescribeGroupBox->isChecked() && !asUser) return;

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  m_trans->GetMatrix(mat);
  mat->SetElement(0, 3, x);
  mat->SetElement(1, 3, y);
  mat->SetElement(2, 3, z);
  m_trans->SetMatrix(mat);

  mat->Delete();
  Modified();
}


void rt2DSliceDataObject::spinRight() {
  spinLeftBy(-1.0f, true);
}

void rt2DSliceDataObject::spinLeft() {
  spinLeftBy(1.0f, true);
}

void rt2DSliceDataObject::rotateUp() {
  rotateUpBy(1.0f, true);
}

void rt2DSliceDataObject::rotateDown() {
  rotateUpBy(-1.0f, true);
}

void rt2DSliceDataObject::rotateLeft() {
  this->rotateLeftBy(1.0, true);
}

void rt2DSliceDataObject::rotateRight() {
  this->rotateLeftBy(-1.0, true);
}

void rt2DSliceDataObject::pushPlane() {
  pushPlaneBy(1.0, true);
}

void rt2DSliceDataObject::pullPlane() {
  pushPlaneBy(-1.0, true);
}

void rt2DSliceDataObject::xTranslate(double v) {
  translateTo(v, m_optionsWidget.yDoubleSpinBox->value(), m_optionsWidget.zDoubleSpinBox->value(), true);
}

void rt2DSliceDataObject::yTranslate(double v) {
  translateTo(m_optionsWidget.xDoubleSpinBox->value(), v, m_optionsWidget.zDoubleSpinBox->value(), true);
}

void rt2DSliceDataObject::zTranslate(double v) {
  translateTo(m_optionsWidget.xDoubleSpinBox->value(), m_optionsWidget.yDoubleSpinBox->value(), v, true);
}

void rt2DSliceDataObject::wlChanged(int w, int l) {
  m_window = w;
  m_level = l;
  Modified();
}

void rt2DSliceDataObject::showWindowLevel() {
  m_wlDialog->show();
}
