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
#include "rt3dVolumeDataObject.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "objTypes.h"
#include "buildParam.h"

#include "rtColorFuncDataObject.h"
#include "rtColorFuncRenderObject.h"
#include "rtPieceFuncDataObject.h"
#include "rtApplication.h"

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

  // Set them both to -1 since at the start we are not using external color or piecewise functions.
  m_piecewiseFuncID = -1;
  m_colorFuncID = -1;

  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  m_volumeProperty->SetColor(m_colorTransFunc);
  m_imgUShortCast->SetInput(m_imgData);
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

bool rt3DVolumeDataObject::copyNewImageData(vtkImageData* temp) {
  if (!temp) return false;

  double rangeI[2], rangeP[2];

  m_imgData->DeepCopy(temp);
  m_imgData->GetScalarRange(rangeI);

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


//! Set the GUI widgets.
void rt3DVolumeDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

  // Window Level
  connect(m_optionsWidget.wlButton, SIGNAL(clicked()), this, SLOT(showWindowLevel()));

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

  // 2D plane reset
  connect(m_optionsWidget.axialResetButton, SIGNAL(clicked()), this, SLOT(axialResetSlot()));
  connect(m_optionsWidget.sagittalResetButton, SIGNAL(clicked()), this, SLOT(sagittalResetSlot()));
  connect(m_optionsWidget.coronalResetButton, SIGNAL(clicked()), this, SLOT(coronalResetSlot()));

  // Watch for new objects to update the lists.
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(newObjectCreated(int)));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(oldObjectRemoved(int)));

  // The combo boxes for the CTF and PWF.
  connect(m_optionsWidget.comboCTFunc, SIGNAL(currentIndexChanged(QString)), this, SLOT(colorTransferChangedGUI(QString)));
  connect(m_optionsWidget.comboPieceFunc, SIGNAL(currentIndexChanged(QString)), this, SLOT(piecewiseChangedGUI(QString)));

  // Iso Value
  connect(m_optionsWidget.isoValueSlider, SIGNAL(valueChanged(int)), this, SLOT(isoValueChanged(int)));

  // Cine
  connect(m_optionsWidget.frameSlider, SIGNAL(valueChanged(int)), this, SLOT(setVisibleComponent(int)));
  connect(m_optionsWidget.cineLoopPushButton, SIGNAL(toggled(bool)), this, SLOT(cineLoop(bool)));

  m_optionsWidget.comboPieceFunc->clear();
  m_optionsWidget.comboPieceFunc->addItem("Default");
  QList<int> pieceFuncs = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_vtkPiecewiseFunction);
  for (int ix1=0; ix1<pieceFuncs.count() ; ix1++) {
    m_optionsWidget.comboPieceFunc->addItem(QString::number(pieceFuncs.at(ix1)));
  }

  m_optionsWidget.comboCTFunc->clear();
  m_optionsWidget.comboCTFunc->addItem("Default");
  QList<int> colorFuncs = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_vtkColorTransferFunction);
  for (int ix1=0; ix1<colorFuncs.count() ; ix1++) {
    m_optionsWidget.comboCTFunc->addItem(QString::number(colorFuncs.at(ix1)));
  }

  m_optionsWidget.interpolateComboBox->setCurrentIndex(m_interpolationType);

}

//! Slot is called when Vurtigo creates a new object.
void rt3DVolumeDataObject::newObjectCreated(int id) {
  rtRenderObject * temp = rtApplication::instance().getObjectManager()->getObjectWithID(id);
  if (temp) {
    if (temp->getObjectType()==rtConstants::OT_vtkPiecewiseFunction) {
      // New Piecewise Function.
      // Do the reset... It may be faster to just add the object but this is more robust.
      m_optionsWidget.comboPieceFunc->clear();
      m_optionsWidget.comboPieceFunc->addItem("Default");
      QList<int> pieceFuncs = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_vtkPiecewiseFunction);
      for (int ix1=0; ix1<pieceFuncs.count() ; ix1++) {
        m_optionsWidget.comboPieceFunc->addItem(QString::number(pieceFuncs.at(ix1)));
      }
    } else if (temp->getObjectType()==rtConstants::OT_vtkColorTransferFunction) {
      // New Color Function.
      m_optionsWidget.comboCTFunc->clear();
      m_optionsWidget.comboCTFunc->addItem("Default");
      QList<int> colorFuncs = rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::OT_vtkColorTransferFunction);
      for (int ix1=0; ix1<colorFuncs.count() ; ix1++) {
        m_optionsWidget.comboCTFunc->addItem(QString::number(colorFuncs.at(ix1)));
      }
    }

  }
}

void rt3DVolumeDataObject::oldObjectRemoved(int id) {

}

void rt3DVolumeDataObject::colorTransferChangedGUI(QString id) {
  bool ok;
  int idInt;

  // Remove the old connection if there is one.
  if(m_colorFuncID != -1) {
    rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_colorFuncID)->getDataObject());
    if (func) {
      disconnect(func, SIGNAL(objectChanged(int)), this, SLOT(colorTransferChanged(int)));
    }
    m_colorFuncID = -1;
  }

  if (id == "Default") {
    // Setup the default CTF.
    m_colorTransFunc = m_colorTransFuncDefault;
    m_colorFuncID = -1;
  } else {

    idInt = id.toInt(&ok);
    if (ok) {
      rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(idInt)->getDataObject());
      m_colorTransFunc = func->getColorFunction();
      m_colorFuncID = idInt;
      connect(func, SIGNAL(objectChanged(int)), this, SLOT(colorTransferChanged(int)));
    }
  }

  m_volumeProperty->SetColor(m_colorTransFunc);
  Modified();
}

void rt3DVolumeDataObject::colorTransferChanged(int id) {
  rtColorFuncDataObject* func = static_cast<rtColorFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(id)->getDataObject());
  if (func) {
    m_colorTransFunc = func->getColorFunction();
    m_volumeProperty->SetColor(m_colorTransFunc);
    Modified();
  }
}

//! A new piecewise function was chosen through the GUI
void rt3DVolumeDataObject::piecewiseChangedGUI(QString id) {
  bool ok;
  int idInt;

  // Remove the old connection if there is one.
  if (m_piecewiseFuncID != -1) {
    rtPieceFuncDataObject* func = static_cast<rtPieceFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(m_piecewiseFuncID)->getDataObject());
    if(func) {   
      disconnect(func, SIGNAL(objectChanged(int)), this, SLOT(piecewiseChanged(int)));
    }
    m_piecewiseFuncID = -1;
  }

  if (id == "Default") {
    // Setup the default PWF.
    m_pieceFunc = m_pieceFuncDefault;
    m_piecewiseFuncID = -1;
  } else {

    idInt = id.toInt(&ok);
    if (ok) {
      rtPieceFuncDataObject* func = static_cast<rtPieceFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(idInt)->getDataObject());
      m_pieceFunc = func->getPiecewiseFunction();
      m_piecewiseFuncID = idInt;
      connect(func, SIGNAL(objectChanged(int)), this, SLOT(piecewiseChanged(int)));
    }
  }
  m_volumeProperty->SetScalarOpacity(m_pieceFunc);
  Modified();
}

void rt3DVolumeDataObject::piecewiseChanged(int id) {
  rtPieceFuncDataObject* func = static_cast<rtPieceFuncDataObject*>(rtApplication::instance().getObjectManager()->getObjectWithID(id)->getDataObject());
  if (func) {
    m_pieceFunc = func->getPiecewiseFunction();
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

//! Clean the GUI widgets.
void rt3DVolumeDataObject::cleanupGUI() {

}

//! Change how the interpolation is done.
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
  Modified();
}

void rt3DVolumeDataObject::nextVisibleComponent() {
  if (!m_imgDataValid) return;
  int numComp = m_imgUShortCast->GetOutput()->GetNumberOfScalarComponents();
  m_visibleComponent = (m_visibleComponent + 1) % numComp;
  m_subImg->SetComponents(m_visibleComponent);

  m_optionsWidget.frameSlider->setValue(m_visibleComponent);

  m_subImg->Update();
  Modified();
}


void rt3DVolumeDataObject::cineLoop(bool cine) {

  if (cine) {
    m_cineFrame->start(100);
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
  m_optionsWidget.volumeInfoTextEdit->clear();
  ts << this->getObjName() << "\n";
  ts << "-------------------------------------------------\n";
  ts << "Range: [0, " << m_window << "] \n";
  ts.flush();

  m_optionsWidget.volumeInfoTextEdit->setPlainText(infoText);
}
