/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "rt3dVolumeDataObject.h"
#include "rt3dVolumeRenderObject.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "objTypes.h"
#include "buildParam.h"
#include "rtApplication.h"
#include "rtMainWindow.h"

#include "rtColorFuncDataObject.h"
#include "rtColorFuncRenderObject.h"
#include "rtPieceFuncDataObject.h"
#include "rtPieceFuncRenderObject.h"
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtTimeManager.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearTransform.h"

rt3DVolumeDataObject::rt3DVolumeDataObject() {
  setObjectType("OT_3DObject");

  m_rayCastFunction=RCF_COMPOSITE;
  m_imgDataValid = false;
  m_annotation = "";

  m_imgUShortCast = vtkImageShiftScale::New();
  m_imgData = vtkImageData::New();
  m_imgClip = vtkImageClip::New();
  m_dataTransform = vtkTransform::New();
  m_pieceFuncDefault = vtkPiecewiseFunction::New();
  m_colorTransFuncDefault = vtkColorTransferFunction::New();
  m_volumeProperty = vtkVolumeProperty::New();

  // The the functions as default for now.
  m_pieceFunc = m_pieceFuncDefault;
  m_colorTransFunc = m_colorTransFuncDefault;

  // The user has yet to change the colors
  m_AColor = m_SColor = m_CColor = false;

  // Set them both to -1 since at the start we are not using external color or piecewise functions.
  m_piecewiseFuncID = -1;
  m_colorFuncID = -1;

  m_imgClip->SetInput(m_imgData);
  m_imgClip->ClipDataOn ();

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);
  m_imgUShortCast->SetInput(m_imgClip->GetOutput());
  m_imgUShortCast->SetOutputScalarTypeToUnsignedShort();

  m_compositeFunc = vtkVolumeRayCastCompositeFunction::New();
  m_isosurfaceFunc = vtkVolumeRayCastIsosurfaceFunction::New();
  m_MIPFunc = vtkVolumeRayCastMIPFunction::New();

  m_visibleComponent = 0;
  m_subImg = vtkImageExtractComponents::New();
  m_subImg->SetInput(m_imgUShortCast->GetOutput());
  m_subImg->SetComponents(m_visibleComponent);

  // Setup the interpolation
  m_interpolationType = 1; // Linear interp

  // Distances are 0
  m_movePoint[0] = 0;
  m_movePoint[1] = 0;
  m_movePoint[2] = 0;

  m_moveOffset = 0;

  m_cineFrame = new QTimer(this);
  connect( m_cineFrame,SIGNAL(timeout()), this, SLOT(nextVisibleComponent()) );

  setupGUI();

  m_wlDialog = new rtWindowLevelDialog( );

  connect(m_wlDialog, SIGNAL(valuesChanged(int,int)), this, SLOT(wlChanged(int,int)));

  connect(this, SIGNAL(newImageData()), this, SLOT(updateInfoText()), Qt::QueuedConnection);

  for (int ix1=0; ix1<3; ix1++) {
    m_planeTransform[ix1] = vtkTransform::New();
  }
}


rt3DVolumeDataObject::~rt3DVolumeDataObject() {
  cleanupGUI();

  if (m_cineFrame && m_cineFrame->isActive()) {
    m_cineFrame->stop();
  }
  if (m_cineFrame) delete m_cineFrame;

  m_imgUShortCast->Delete();
  m_imgData->Delete();
  m_imgClip->Delete();
  m_dataTransform->Delete();
  m_pieceFuncDefault->Delete();
  m_colorTransFuncDefault->Delete();
  m_volumeProperty->Delete();

  m_compositeFunc->Delete();
  m_isosurfaceFunc->Delete();
  m_MIPFunc->Delete();
  m_subImg->Delete();

  if (m_wlDialog) delete m_wlDialog;

  for (int ix1=0; ix1<3; ix1++) {
    m_planeTransform[ix1]->Delete();
  }
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

vtkImageData* rt3DVolumeDataObject::getImageData() {
  return m_imgData;
}

vtkImageData* rt3DVolumeDataObject::getUShortData() {
  return m_subImg->GetOutput();
}


vtkTransform* rt3DVolumeDataObject::getTransform() {
  return m_dataTransform;
}

vtkPiecewiseFunction* rt3DVolumeDataObject::getPieceFunc() {
  return m_pieceFunc;
}

vtkColorTransferFunction* rt3DVolumeDataObject::getColorTransFunc() {
  return m_colorTransFunc;
}

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

bool rt3DVolumeDataObject::copyNewImageData(vtkImageData* temp,int type) {
  if (!temp) return false;

  double rangeI[2], rangeP[2];
  int wholeExtent[6];

  temp->GetWholeExtent(wholeExtent);
  m_imgClip->SetOutputWholeExtent(wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5]);

  m_optionsWidget.xminSlider->setMinimum(wholeExtent[0]);
  m_optionsWidget.xminSlider->setMaximum(wholeExtent[1]);
  m_optionsWidget.xminSlider->setValue(wholeExtent[0]);
  m_optionsWidget.xminLabel->setText(QString::number(wholeExtent[0]));

  m_optionsWidget.xmaxSlider->setMinimum(wholeExtent[0]);
  m_optionsWidget.xmaxSlider->setMaximum(wholeExtent[1]);
  m_optionsWidget.xmaxSlider->setValue(wholeExtent[1]);
  m_optionsWidget.xmaxLabel->setText(QString::number(wholeExtent[1]));

  m_optionsWidget.yminSlider->setMinimum(wholeExtent[2]);
  m_optionsWidget.yminSlider->setMaximum(wholeExtent[3]);
  m_optionsWidget.yminSlider->setValue(wholeExtent[2]);
  m_optionsWidget.yminLabel->setText(QString::number(wholeExtent[2]));

  m_optionsWidget.ymaxSlider->setMinimum(wholeExtent[2]);
  m_optionsWidget.ymaxSlider->setMaximum(wholeExtent[3]);
  m_optionsWidget.ymaxSlider->setValue(wholeExtent[3]);
  m_optionsWidget.ymaxLabel->setText(QString::number(wholeExtent[3]));

  m_optionsWidget.zminSlider->setMinimum(wholeExtent[4]);
  m_optionsWidget.zminSlider->setMaximum(wholeExtent[5]);
  m_optionsWidget.zminSlider->setValue(wholeExtent[4]);
  m_optionsWidget.zminLabel->setText(QString::number(wholeExtent[4]));

  m_optionsWidget.zmaxSlider->setMinimum(wholeExtent[4]);
  m_optionsWidget.zmaxSlider->setMaximum(wholeExtent[5]);
  m_optionsWidget.zmaxSlider->setValue(wholeExtent[5]);
  m_optionsWidget.zmaxLabel->setText(QString::number(wholeExtent[5]));

  m_imgData->DeepCopy(temp);
  m_imgData->GetScalarRange(rangeI);
  m_imgType = type;

  m_imgUShortCast->SetShift(-rangeI[0]);
  m_imgUShortCast->Update();
  m_imgUShortCast->GetOutput()->GetScalarRange(rangeP);

  m_wlDialog->setImageData(m_imgUShortCast->GetOutput());

  m_pieceFuncDefault->RemoveAllPoints();
  m_pieceFuncDefault->AddPoint(rangeP[0], 0.0);
  m_pieceFuncDefault->AddPoint(rangeP[1], 1.0);

  m_colorTransFuncDefault->RemoveAllPoints();
  m_colorTransFuncDefault->AddRGBPoint(rangeP[0], 0.0, 0.0, 0.0);
  m_colorTransFuncDefault->AddRGBPoint(rangeP[1], 1.0, 0.5, 0.5);

  m_window = (rangeI[1]-rangeI[0]);
  m_level = (rangeI[1]+rangeI[0])/2.0f;

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);

  if (m_imgUShortCast->GetOutput()->GetNumberOfScalarComponents() > 1) {
    m_optionsWidget.frameSlider->setMinimum(0);
    m_optionsWidget.frameSlider->setMaximum(m_imgUShortCast->GetOutput()->GetNumberOfScalarComponents()-1);
    m_optionsWidget.cineGroupBox->setDisabled(false);
  } else {
    m_optionsWidget.cineGroupBox->setDisabled(true);
  }

  m_isosurfaceFunc->SetIsoValue(m_level);
  m_optionsWidget.isoValueSlider->setMinimum(0);
  m_optionsWidget.isoValueSlider->setMaximum(m_window);
  m_optionsWidget.isoValueSlider->setValue(m_level);

  m_imgDataValid = true;
  emit newImageData();
  Modified();
  return true;
}


bool rt3DVolumeDataObject::copyNewTransform(vtkTransform* temp) {
  if (!temp) return false;

  m_dataTransform->SetMatrix(temp->GetMatrix());

  return true;
}

void rt3DVolumeDataObject::translateData(double x, double y, double z) {
  m_dataTransform->Translate(x,y,z);
}

void rt3DVolumeDataObject::scaleData(double x, double y, double z) {
  m_dataTransform->Scale(x,y,z);
}

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

void rt3DVolumeDataObject::colorTransferChanged(int id) {
    if (id == -1)
    {
        m_colorTransFunc = m_colorTransFuncDefault;
        m_volumeProperty->SetColor(m_colorTransFunc);
        m_colorFuncID = -1;
        Modified();
        return;
    }
  rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(id)->getDataObject());
  if (func) {
    m_colorTransFunc = func->getColorFunction();
    m_colorFuncID = id;
    m_volumeProperty->SetColor(m_colorTransFunc);
    Modified();
  }
}


void rt3DVolumeDataObject::piecewiseChanged(int id) {
    if (id == -1)
    {
        m_pieceFunc = m_pieceFuncDefault;
        m_volumeProperty->SetScalarOpacity(m_pieceFunc);
        m_piecewiseFuncID = -1;
        Modified();
        return;
    }
  rtPieceFuncDataObject* func = static_cast<rtPieceFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(id)->getDataObject());
  if (func) {
    m_pieceFunc = func->getPiecewiseFunction();
    m_piecewiseFuncID = id;
    m_volumeProperty->SetScalarOpacity(m_pieceFunc);
    Modified();
  }
}

void rt3DVolumeDataObject::showWindowLevel() {
  m_wlDialog->show();
}

void rt3DVolumeDataObject::wlChanged(int w, int l) {
  m_window = w;
  m_level = l;
  Modified();
}

void rt3DVolumeDataObject::axialResetSlot() {
  emit axialResetSignal();
  Modified();
}

void rt3DVolumeDataObject::sagittalResetSlot() {
  emit sagittalResetSignal();
  Modified();
}

void rt3DVolumeDataObject::coronalResetSlot() {
  emit coronalResetSignal();
  Modified();
}

void rt3DVolumeDataObject::axialColorChanged()
{
    QColor clr = QColorDialog::getColor(m_axialColor);
    if (clr.isValid())
        setAxialColor(clr);
}

void rt3DVolumeDataObject::setAxialColor(QColor clr)
{
        m_axialColor = clr;
        QPalette palette;
        palette.setColor(m_optionsWidget.axialColor->backgroundRole(), clr);
        m_optionsWidget.axialColor->setPalette(palette);
        m_AColor = true;
        Modified();
}

void rt3DVolumeDataObject::sagittalColorChanged()
{
    QColor clr = QColorDialog::getColor(m_sagittalColor);
    if (clr.isValid())
        setSagittalColor(clr);
}

void rt3DVolumeDataObject::setSagittalColor(QColor clr)
{
        m_sagittalColor = clr;
        QPalette palette;
        palette.setColor(m_optionsWidget.sagittalColor->backgroundRole(), clr);
        m_optionsWidget.sagittalColor->setPalette(palette);
        m_SColor = true;
        Modified();
}

void rt3DVolumeDataObject::coronalColorChanged()
{
    QColor clr = QColorDialog::getColor(m_coronalColor);
    if (clr.isValid())
        setCoronalColor(clr);
}

void rt3DVolumeDataObject::setCoronalColor(QColor clr)
{
        m_coronalColor = clr;
        QPalette palette;
        palette.setColor(m_optionsWidget.coronalColor->backgroundRole(), clr);
        m_optionsWidget.coronalColor->setPalette(palette);
        m_CColor = true;
        Modified();
}

void rt3DVolumeDataObject::interpolationChanged(int interp) {
  m_interpolationType = interp;
  Modified();
}


void rt3DVolumeDataObject::setVisibleComponent(int c) {
  if (!m_imgDataValid) return;
  if (m_visibleComponent == c) return;
  int numComp = m_imgUShortCast->GetOutput()->GetNumberOfScalarComponents();

  if (c < 0 || c >= numComp) {
    m_visibleComponent = 0;
  } else {
    m_visibleComponent = c;
  }
  m_subImg->SetComponents(m_visibleComponent);
  m_subImg->Update();
  m_optionsWidget.frameSlider->setValue(m_visibleComponent);
  Modified();
}

void rt3DVolumeDataObject::nextVisibleComponent() {
  if (!m_imgDataValid) return;
  int trigDelay = rtApplication::instance().getTimeManager()->getTriggerDelay();

  int phase;
  double diff=10000;
  for (int ix1=0; ix1<m_triggerList.size(); ix1++) {
    if (fabs(m_triggerList[ix1]-(double)trigDelay) < diff) {
      diff = fabs(m_triggerList[ix1]-(double)trigDelay);
      phase = ix1;
    }
  }
  if (m_visibleComponent != phase) setVisibleComponent(phase);
}


void rt3DVolumeDataObject::cineLoop(bool cine) {

  if (cine) {
    m_cineFrame->start(20);
  } else if (m_cineFrame->isActive()) {
    m_cineFrame->stop();
  }

}

void rt3DVolumeDataObject::isoValueChanged(int v) {
  if (m_isosurfaceFunc->GetIsoValue() != v) {
    m_isosurfaceFunc->SetIsoValue(v);
    Modified();
  }
  m_optionsWidget.isoValueLabel->setText(QString::number(v));
}

void rt3DVolumeDataObject::updateInfoText() {
  QString infoText;
  QTextStream ts(&infoText, QIODevice::WriteOnly);

  double rangeP[2];
  m_imgUShortCast->GetOutput()->GetScalarRange(rangeP);

  int dims[3];
  m_imgUShortCast->GetOutput()->GetDimensions( dims );

  double spacing[3];
  m_imgUShortCast->GetOutput()->GetSpacing(spacing);

  m_optionsWidget.volumeInfoTextEdit->clear();
  ts << this->getObjName() << "\n";
  ts << "--------------------------------------------\n";
  ts << "Study ID:        [" << m_commonData.getStudyID() << "] \n";
  ts << "Series Number:   [" << m_commonData.getSeriesNumber() << "] \n";
  ts << "Study Date:      " << m_commonData.getStudyDate().toString("dd.MM.yyyy") << " \n";
  ts << "Study Time:      " << m_commonData.getStudyTime().toString("hh:mm:ss") << " \n";
  ts << "Scalar Range:    [" << rangeP[0] <<", " << rangeP[1] << "] \n";
  ts << "Volume Size:     [" << dims[0] << ", " << dims[1] << ", " << dims[2] << "] \n";
  ts << "Number of Frames:" << m_imgUShortCast->GetOutput()->GetNumberOfScalarComponents() << " \n";
  ts << "Memory Used:     " << m_imgUShortCast->GetOutput()->GetActualMemorySize() << " kB \n";
  ts << "Slice Spacing:   [" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "] \n";
  ts << "Patient Position:[" << m_commonData.getPatientPosition() << "] \n";
  ts.flush();

  m_optionsWidget.volumeInfoTextEdit->setPlainText(infoText);
  m_optionsWidget.volumeInfoTextEdit->setFont(QFont("Courier"));

  // Now update the annotation text.
  QTextStream annotate(&m_annotation, QIODevice::WriteOnly);
  annotate << "-------------------------------\n";
  annotate << this->getObjName() << "\n";
  annotate << "-------------------------------\n";
  annotate << "Study ID:        " << m_commonData.getStudyID() << " \n";
  annotate << "Series Number:   " << m_commonData.getSeriesNumber() << " \n";
  annotate << "Study Date:      " << m_commonData.getStudyDate().toString("dd.MM.yyyy") << " \n";
  annotate << "Study Time:      " << m_commonData.getStudyTime().toString("hh:mm:ss") << " \n";
  annotate.flush();

  //Change Coordinate Axes depending on patient position info in the Volume
  QString ct = m_commonData.getPatientPosition();
  if (ct == "HFS")
       rtApplication::instance().getMainWinHandle()->setCoordManual(0);
  else if (ct == "FFS")
      rtApplication::instance().getMainWinHandle()->setCoordManual(1);
  else if (ct == "HFP")
      rtApplication::instance().getMainWinHandle()->setCoordManual(2);
  else if (ct == "FFP")
      rtApplication::instance().getMainWinHandle()->setCoordManual(3);
  else if (ct == "HFDR")
      rtApplication::instance().getMainWinHandle()->setCoordManual(4);
  else if (ct == "FFDR")
      rtApplication::instance().getMainWinHandle()->setCoordManual(5);
  else if (ct == "HFDL")
      rtApplication::instance().getMainWinHandle()->setCoordManual(6);
  else if (ct == "FFDL")
      rtApplication::instance().getMainWinHandle()->setCoordManual(7);
  else
      rtApplication::instance().getMainWinHandle()->setCoordManual(8);
}

void rt3DVolumeDataObject::cropStatusChanged(bool status) {
  int ext[6];
  if (status) {
    ext[0] = m_optionsWidget.xminSlider->value();
    ext[1] = m_optionsWidget.xmaxSlider->value();
    ext[2] = m_optionsWidget.yminSlider->value();
    ext[3] = m_optionsWidget.ymaxSlider->value();
    ext[4] = m_optionsWidget.zminSlider->value();
    ext[5] = m_optionsWidget.zmaxSlider->value();
    m_imgClip->SetOutputWholeExtent(ext);
  } else {
    m_imgData->GetWholeExtent(ext);
    m_imgClip->SetOutputWholeExtent(ext);
  }
  Modified();
}

void rt3DVolumeDataObject::xminSliderChanged(int val) {
  // Adjust the max to fit be at least the min.
  if (m_optionsWidget.xmaxSlider->value() < val) {
    m_optionsWidget.xmaxSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[0] != val) {
    ext[0] = val;
    m_optionsWidget.xminLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::xmaxSliderChanged(int val) {
  // Adjust the min to fit the max.
  if (m_optionsWidget.xminSlider->value() > val) {
    m_optionsWidget.xminSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[1] != val) {
    ext[1] = val;
    m_optionsWidget.xmaxLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::yminSliderChanged(int val) {
  // Adjust the max to fit be at least the min.
  if (m_optionsWidget.ymaxSlider->value() < val) {
    m_optionsWidget.ymaxSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[2] != val) {
    ext[2] = val;
    m_optionsWidget.yminLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::ymaxSliderChanged(int val) {
  // Adjust the min to fit the max.
  if (m_optionsWidget.yminSlider->value() > val) {
    m_optionsWidget.yminSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[3] != val) {
    ext[3] = val;
    m_optionsWidget.ymaxLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::zminSliderChanged(int val) {
  // Adjust the max to fit be at least the min.
  if (m_optionsWidget.zmaxSlider->value() < val) {
    m_optionsWidget.zmaxSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[4] != val) {
    ext[4] = val;
    m_optionsWidget.zminLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::zmaxSliderChanged(int val) {
  // Adjust the min to fit the max.
  if (m_optionsWidget.zminSlider->value() > val) {
    m_optionsWidget.zminSlider->setValue(val);
  }

  int ext[6];
  m_imgClip->GetOutputWholeExtent(ext);
  if (ext[5] != val) {
    ext[5] = val;
    m_optionsWidget.zmaxLabel->setText(QString::number(val));
    m_imgClip->SetOutputWholeExtent(ext);
    Modified();
  }
}

void rt3DVolumeDataObject::boundBoxChanged(bool status) {
    emit boundBoxSignal(status);
    Modified();
}


void rt3DVolumeDataObject::createNewPWF() {
  rtObjectManager* oManage = rtApplication::instance().getObjectManager();
  rtPieceFuncRenderObject* pieceFunc=NULL;
  rtPieceFuncDataObject* datObj=NULL;
  int place;

  // Check the object manager poiner
  if (!oManage) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Object Manager pointer is NULL!"));
    return;
  }

  pieceFunc = static_cast<rtPieceFuncRenderObject*>(oManage->addObjectOfType("OT_vtkPiecewiseFunction", this->getObjName().append("_PWF")));
  if (!pieceFunc) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to create piecewise function. Action aborted."));
    return;
  }

  datObj = static_cast<rtPieceFuncDataObject*>(pieceFunc->getDataObject());
  if(!datObj) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to get piecewise function data. Action aborted."));
    return;
  }

  if (!datObj->setPiecewiseFunction(m_pieceFuncDefault)) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Piecewise function default was not set."));
  }

}


void rt3DVolumeDataObject::createNewCTF() {
  rtObjectManager* oManage = rtApplication::instance().getObjectManager();
  rtColorFuncRenderObject* colorFunc=NULL;
  rtColorFuncDataObject* datObj=NULL;
  int place;

  // Check the object manager poiner
  if (!oManage) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Object Manager pointer is NULL!"));
    return;
  }

  colorFunc = static_cast<rtColorFuncRenderObject*>(oManage->addObjectOfType("OT_vtkColorTransferFunction", this->getObjName().append("_CTF")));
  if (!colorFunc) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to create color function. Action aborted."));
    return;
  }

  datObj = static_cast<rtColorFuncDataObject*>(colorFunc->getDataObject());
  if(!datObj) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to get color function data. Action aborted."));
    return;
  }

  if (!datObj->setColorFunction(m_colorTransFuncDefault)) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Color function default was not set."));
  }

}

///////////////////
// Protected Functions
///////////////////


void rt3DVolumeDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  // Annotation
  m_optionsWidget.annotateCheckBox->setChecked(true);
  connect(m_optionsWidget.annotateCheckBox, SIGNAL(toggled(bool)), this, SLOT(Modified()));

  // Volume Crop.
  m_optionsWidget.cropVolumeGroupBox->setChecked(true);

  connect(m_optionsWidget.cropVolumeGroupBox, SIGNAL(clicked(bool)), this, SLOT(cropStatusChanged(bool)));
  connect(m_optionsWidget.xminSlider, SIGNAL(valueChanged(int)), this, SLOT(xminSliderChanged(int)));
  connect(m_optionsWidget.xmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(xmaxSliderChanged(int)));
  connect(m_optionsWidget.yminSlider, SIGNAL(valueChanged(int)), this, SLOT(yminSliderChanged(int)));
  connect(m_optionsWidget.ymaxSlider, SIGNAL(valueChanged(int)), this, SLOT(ymaxSliderChanged(int)));
  connect(m_optionsWidget.zminSlider, SIGNAL(valueChanged(int)), this, SLOT(zminSliderChanged(int)));
  connect(m_optionsWidget.zmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(zmaxSliderChanged(int)));

  // Pickable volume
  connect(m_optionsWidget.volumePickableCheckBox, SIGNAL(toggled(bool)), this, SLOT(Modified()));

  // GPU Enable
  connect(m_optionsWidget.gpuBox,SIGNAL(toggled(bool)),this, SLOT(gpuBoxChecked(bool)));

  // Window Level
  connect(m_optionsWidget.wlButton, SIGNAL(clicked()), this, SLOT(showWindowLevel()));

  // Interpolation Type.
  connect(m_optionsWidget.interpolateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(interpolationChanged(int)));

  // Global ray cast volume switch
  connect(m_optionsWidget.groupRayCastVolume, SIGNAL(toggled(bool)), this, SLOT(Modified()));

  // Bounding Box switch
  connect(m_optionsWidget.boundBox, SIGNAL(toggled(bool)), this, SLOT(boundBoxChanged(bool)));

  // Surface functions.
  connect(m_optionsWidget.radioComposite, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));
  connect(m_optionsWidget.radioIsosurface, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));
  connect(m_optionsWidget.radioMIP, SIGNAL(toggled(bool)), this, SLOT(surfaceFunctionChanged()));

  // 2D plane check boxes
  connect(m_optionsWidget.checkAxial3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));
  connect(m_optionsWidget.checkSagittal3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));
  connect(m_optionsWidget.checkCoronal3D, SIGNAL(toggled(bool)), this, SLOT(Modified()));

  // 2D plane reset
  connect(m_optionsWidget.axialResetButton, SIGNAL(clicked()), this, SLOT(axialResetSlot()));
  connect(m_optionsWidget.sagittalResetButton, SIGNAL(clicked()), this, SLOT(sagittalResetSlot()));
  connect(m_optionsWidget.coronalResetButton, SIGNAL(clicked()), this, SLOT(coronalResetSlot()));

  // 2D plane opacity sliders
  connect(m_optionsWidget.axialOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(Modified()));
  connect(m_optionsWidget.sagittalOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(Modified()));
  connect(m_optionsWidget.coronalOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(Modified()));

  //2D plane colors
  connect(m_optionsWidget.axialColor, SIGNAL(clicked()), this, SLOT(axialColorChanged()));
  connect(m_optionsWidget.sagittalColor, SIGNAL(clicked()), this, SLOT(sagittalColorChanged()));
  connect(m_optionsWidget.coronalColor, SIGNAL(clicked()), this, SLOT(coronalColorChanged()));

  // The combo boxes for the CTF and PWF.
  connect(&m_PWFunc, SIGNAL(objectSelectionChanged(int)), this, SLOT(piecewiseChanged(int)));
  connect(&m_CTFunc, SIGNAL(objectSelectionChanged(int)), this, SLOT(colorTransferChanged(int)));

  // Creating new CTF and PWF
  connect(m_optionsWidget.newCTF, SIGNAL(clicked()), this, SLOT(createNewCTF()));
  connect(m_optionsWidget.newPWF, SIGNAL(clicked()), this, SLOT(createNewPWF()));

  // Iso Value
  connect(m_optionsWidget.isoValueSlider, SIGNAL(valueChanged(int)), this, SLOT(isoValueChanged(int)));

  // Cine
  connect(m_optionsWidget.frameSlider, SIGNAL(valueChanged(int)), this, SLOT(setVisibleComponent(int)));
  connect(m_optionsWidget.cineLoopPushButton, SIGNAL(toggled(bool)), this, SLOT(cineLoop(bool)));

  // Volume Moving
  connect(m_optionsWidget.volumeCopyButton, SIGNAL(clicked()),this,SLOT(copyVolume()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(setupVolumes()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(setupVolumes()));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectRenamed(int)), this, SLOT(setupVolumes()));

  m_PWFunc.addObjectType("OT_vtkPiecewiseFunction");
  static_cast<QGridLayout*>(m_optionsWidget.groupRayCastVolume->layout())->addWidget(&m_PWFunc,5,1);

  m_CTFunc.addObjectType("OT_vtkColorTransferFunction");
  static_cast<QGridLayout*>(m_optionsWidget.groupRayCastVolume->layout())->addWidget(&m_CTFunc,6,1);

  m_optionsWidget.interpolateComboBox->setCurrentIndex(m_interpolationType);

  m_optionsWidget.axialColor->setAutoFillBackground(true);
  m_optionsWidget.sagittalColor->setAutoFillBackground(true);
  m_optionsWidget.coronalColor->setAutoFillBackground(true);

  m_axialColor = QColor("white");
  m_sagittalColor = QColor("white");
  m_coronalColor = QColor("white");
}

void rt3DVolumeDataObject::setupGPUGUI()
{
    m_optionsWidget.gpuBox->setEnabled(m_canGPU);
    if (m_canGPU)
    {
        m_optionsWidget.radioComposite->setEnabled(!m_gpuBoxOn);
        m_optionsWidget.radioIsosurface->setEnabled(!m_gpuBoxOn);
        m_optionsWidget.radioMIP->setEnabled(!m_gpuBoxOn);
        m_optionsWidget.isoValueSlider->setEnabled(!m_gpuBoxOn);
        m_optionsWidget.isoValueLabel->setEnabled(!m_gpuBoxOn);
        m_optionsWidget.label_5->setEnabled(!m_gpuBoxOn);
    }
}


void rt3DVolumeDataObject::cleanupGUI() {

}

bool rt3DVolumeDataObject::saveFile(QFile* file)
{
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
      return false;
    rt3DVolumeRenderObject *rObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(this->getId()));
    QXmlStreamWriter writer(file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("VurtigoFile");
    rtDataObject::saveHeader(&writer, getObjectType(), getObjName());
    rObj->saveVisibilities(&writer);

    for (int ix1=0; ix1<m_triggerList.size(); ++ix1)
    {
        writer.writeTextElement("TriggerDelay",QString::number(m_triggerList[ix1]));
    }
    writer.writeTextElement("ImageType",QString::number(m_imgType));

    this->saveTransformToXML("VolTransform",this->getTransform(),&writer);

    writer.writeTextElement("window",QString::number(this->getWindow()));
    writer.writeTextElement("level",QString::number(this->getLevel()));
    writer.writeTextElement("phase",QString::number(this->getVisibleComponent()));

    writer.writeStartElement("Axial");
    writer.writeAttribute("Visibility",QString::number(this->getAxial3D()));
    writer.writeAttribute("opacity",QString::number(this->getAxialOpacity()));
    writer.writeAttribute("color",this->getAxialColor().name());
    this->saveTransformToXML("PlaneTransform",rObj->getBoxOutlineTransform(0),&writer);
    writer.writeEndElement();

    writer.writeStartElement("Sagittal");
    writer.writeAttribute("Visibility",QString::number(this->getSagittal3D()));
    writer.writeAttribute("opacity",QString::number(this->getSagittalOpacity()));
    writer.writeAttribute("color",this->getSagittalColor().name());
    this->saveTransformToXML("PlaneTransform",rObj->getBoxOutlineTransform(1),&writer);
    writer.writeEndElement();

    writer.writeStartElement("Coronal");
    writer.writeAttribute("Visibility",QString::number(this->getCoronal3D()));
    writer.writeAttribute("opacity",QString::number(this->getCoronalOpacity()));
    writer.writeAttribute("color",this->getCoronalColor().name());
    this->saveTransformToXML("PlaneTransform",rObj->getBoxOutlineTransform(2),&writer);
    writer.writeEndElement();

    writer.writeEndElement(); //VurtigoFile
    writer.writeEndDocument();
    file->close();

    vtkXMLImageDataWriter *imgWriter = vtkXMLImageDataWriter::New();
    imgWriter->SetInput(this->getImageData());
    imgWriter->SetFileName(QString(file->fileName() + "_img").toStdString().c_str());
    imgWriter->Write();
    imgWriter->Delete();

    //this->getTransform()->GetNumberOfConcatenatedTransforms()
    return true;
}

bool rt3DVolumeDataObject::loadFile(QFile *file)
{
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
      return false;

    QXmlStreamReader reader(file);
    QString objType;
    QString objName = "";
    QList<double> trigs;
    vtkTransform *trans = vtkTransform::New();
    vtkTransform *Atrans = vtkTransform::New();
    vtkTransform *Strans = vtkTransform::New();
    vtkTransform *Ctrans = vtkTransform::New();
    trans->Identity();
    Atrans->Identity();
    Strans->Identity();
    Ctrans->Identity();
    int type;
    int window,level,phase;
    bool Avis,Svis,Cvis;
    double Aopac,Sopac,Copac;
    QString Acolor,Scolor,Ccolor;
    QXmlStreamAttributes attribs;

    rt3DVolumeRenderObject *rObj = static_cast<rt3DVolumeRenderObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(this->getId()));

    while (!reader.atEnd())
    {
        if (reader.readNext() == QXmlStreamReader::StartElement)
        {
            if (reader.name() == "FileInfo")
            {
                rtDataObject::loadHeader(&reader,objType,objName);
                if ( objType != this->getObjectType() ) {
                  rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Object Type for data file is wrong."));
                  break;
                }
            }
            else if (reader.name() == "Visibilities")
            {
                rObj->loadVisibilities(&reader);
            }
            else if (reader.name() == "TriggerDelay")
            {
                trigs.append(reader.readElementText().toDouble());
            }
            else if (reader.name() == "ImageType")
            {
                type = reader.readElementText().toInt();
            }
            else if (reader.name() == "VolTransform")
            {
                this->loadTransformFromXML(trans,&reader);
            }
            else if (reader.name() == "window")
            {
                window = reader.readElementText().toInt();
            }
            else if (reader.name() == "level")
            {
                level = reader.readElementText().toInt();
            }
            else if (reader.name() == "phase")
            {
                phase = reader.readElementText().toInt();
            }
            else if (reader.name() == "Axial")
            {
                attribs = reader.attributes();
                Avis = attribs.value("Visibility").toString().toInt();
                Aopac = attribs.value("opacity").toString().toDouble();
                Acolor = attribs.value("color").toString();
                while(true)
                {
                    reader.readNext();
                    if (reader.name() == "PlaneTransform")
                    {
                        this->loadTransformFromXML(Atrans,&reader);
                        break;
                    }
                }
            }
            else if (reader.name() == "Sagittal")
            {
                attribs = reader.attributes();
                Svis = attribs.value("Visibility").toString().toInt();
                Sopac = attribs.value("opacity").toString().toDouble();
                Scolor = attribs.value("color").toString();
                while(true)
                {
                    reader.readNext();
                    if (reader.name() == "PlaneTransform")
                    {
                        this->loadTransformFromXML(Strans,&reader);
                        break;
                    }
                }
            }
            else if (reader.name() == "Coronal")
            {
                attribs = reader.attributes();
                Cvis = attribs.value("Visibility").toString().toInt();
                Copac = attribs.value("opacity").toString().toDouble();
                Ccolor = attribs.value("color").toString();
                while(true)
                {
                    reader.readNext();
                    if (reader.name() == "PlaneTransform")
                    {
                        this->loadTransformFromXML(Ctrans,&reader);
                        break;
                    }
                }
            }
        }
    }
    if (reader.hasError())
    {
        file->close();
        return false;
    }

    file->close();
    vtkXMLImageDataReader *imgReader = vtkXMLImageDataReader::New();
    if (imgReader->CanReadFile(QString(file->fileName() + "_img").toStdString().c_str()))
    {
        imgReader->SetFileName(QString(file->fileName() + "_img").toStdString().c_str());
        imgReader->Update();
        this->lock();
        this->copyNewTransform(trans);
        this->copyTriggerDelayList(&trigs);
        this->copyNewImageData(imgReader->GetOutput(),type);
        this->setAxial3D(Avis);
        this->setAxialOpacity(Aopac);
        this->setAxialColor(QColor(Acolor));
        this->setSagittal3D(Svis);
        this->setSagittalOpacity(Sopac);
        this->setSagittalColor(QColor(Scolor));
        this->setCoronal3D(Cvis);
        this->setCoronalOpacity(Copac);
        this->setCoronalColor(QColor(Ccolor));
        m_wlDialog->windowSlider->setValue(window*1000);
        m_wlDialog->windowSliderChange(window*1000);
        m_wlDialog->levelSlider->setValue(level*1000);
        m_wlDialog->levelSliderChange(level*1000);
        this->setVisibleComponent(phase);
        this->unlock();

        rObj->setBoxOutlineTransform(0,Atrans);
        rObj->setTexturePlaneTransform(0,Atrans);
        rObj->setPlaneControlTransform(0,Atrans);
        rObj->setBoxOutlineTransform(1,Strans);
        rObj->setTexturePlaneTransform(1,Strans);
        rObj->setPlaneControlTransform(1,Strans);
        rObj->setBoxOutlineTransform(2,Ctrans);
        rObj->setTexturePlaneTransform(2,Ctrans);
        rObj->setPlaneControlTransform(2,Ctrans);

        imgReader->Delete();
        return true;
    }
    else
    {
        imgReader->Delete();
        return false;
    }

    trans->Delete();
    Atrans->Delete();
    Strans->Delete();
    Ctrans->Delete();
}

void rt3DVolumeDataObject::setupVolumes()
{
    m_optionsWidget.volumeBox->clear();
    QList<int> vols = rtApplication::instance().getObjectManager()->getObjectsOfType("OT_3DObject");
    rt3DVolumeDataObject *v;
    for (int ix1=0; ix1<vols.size(); ix1++)
    {
        v = static_cast<rt3DVolumeDataObject *>(rtApplication::instance().getObjectManager()->getObjectWithID(vols[ix1])->getDataObject());
        m_optionsWidget.volumeBox->addItem(v->getObjName() + " " + QString::number(vols[ix1]));
    }
}

void rt3DVolumeDataObject::copyVolume()
{
    QList<int> vols = rtApplication::instance().getObjectManager()->getObjectsOfType("OT_3DObject");
    rt3DVolumeDataObject *v;
    v = static_cast<rt3DVolumeDataObject *>(rtApplication::instance().getObjectManager()->getObjectWithID(vols.at(m_optionsWidget.volumeBox->currentIndex()))->getDataObject());
    this->lock();
    this->copyNewTransform(v->getTransform());
    this->getImageData()->SetSpacing(v->getImageData()->GetSpacing());
    //this->copyNewImageData(this->getImageData(),this->m_imgType);
    this->unlock();
    this->Modified();
}

void rt3DVolumeDataObject::saveTransformToXML(QString name, vtkTransform *trans,QXmlStreamWriter *writer )
{
    for (int ix1=0; ix1<trans->GetNumberOfConcatenatedTransforms(); ix1++)
    {
        writer->writeStartElement(name);
        writer->writeAttribute("a00",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(0,0)));
        writer->writeAttribute("a01",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(0,1)));
        writer->writeAttribute("a02",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(0,2)));
        writer->writeAttribute("a03",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(0,3)));
        writer->writeAttribute("a10",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(1,0)));
        writer->writeAttribute("a11",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(1,1)));
        writer->writeAttribute("a12",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(1,2)));
        writer->writeAttribute("a13",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(1,3)));
        writer->writeAttribute("a20",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(2,0)));
        writer->writeAttribute("a21",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(2,1)));
        writer->writeAttribute("a22",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(2,2)));
        writer->writeAttribute("a23",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(2,3)));
        writer->writeAttribute("a30",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(3,0)));
        writer->writeAttribute("a31",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(3,1)));
        writer->writeAttribute("a32",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(3,2)));
        writer->writeAttribute("a33",QString::number(trans->GetConcatenatedTransform(ix1)->GetMatrix()->GetElement(3,3)));
        writer->writeEndElement();

    }
}

void rt3DVolumeDataObject::loadTransformFromXML(vtkTransform *trans, QXmlStreamReader *reader)
{
    QXmlStreamAttributes attribs = reader->attributes();
    vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
    mat->SetElement(0,0,attribs.value("a00").toString().toDouble());
    mat->SetElement(0,1,attribs.value("a01").toString().toDouble());
    mat->SetElement(0,2,attribs.value("a02").toString().toDouble());
    mat->SetElement(0,3,attribs.value("a03").toString().toDouble());
    mat->SetElement(1,0,attribs.value("a10").toString().toDouble());
    mat->SetElement(1,1,attribs.value("a11").toString().toDouble());
    mat->SetElement(1,2,attribs.value("a12").toString().toDouble());
    mat->SetElement(1,3,attribs.value("a13").toString().toDouble());
    mat->SetElement(2,0,attribs.value("a20").toString().toDouble());
    mat->SetElement(2,1,attribs.value("a21").toString().toDouble());
    mat->SetElement(2,2,attribs.value("a22").toString().toDouble());
    mat->SetElement(2,3,attribs.value("a23").toString().toDouble());
    mat->SetElement(3,0,attribs.value("a30").toString().toDouble());
    mat->SetElement(3,1,attribs.value("a31").toString().toDouble());
    mat->SetElement(3,2,attribs.value("a32").toString().toDouble());
    mat->SetElement(3,3,attribs.value("a33").toString().toDouble());
    trans->Concatenate(mat);
}
