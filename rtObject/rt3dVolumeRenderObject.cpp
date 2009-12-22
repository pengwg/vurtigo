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
#include "rt3dVolumeRenderObject.h"
#include "rt3dVolumeDataObject.h"
#include "rtMainWindow.h"
#include "rtObjectManager.h"

#include <math.h>

#include <vtkProperty.h>
#include <vtkMath.h>

rt3DVolumeRenderObject::rt3DVolumeRenderObject() {
  setObjectType(rtConstants::OT_3DObject);
  setName("3D Volume Renderer");

  m_imgReslice[0] = vtkImageReslice::New();
  m_imgReslice[1] = vtkImageReslice::New();
  m_imgReslice[2] = vtkImageReslice::New();

  m_currentPlane = -1;
  m_isInit = false;
  m_selectedProp = NULL;

  setupDataObject();
  setupPipeline();
}


rt3DVolumeRenderObject::~rt3DVolumeRenderObject() {

  for (int ix1=0; ix1<3; ix1++) {
    m_imgCast[ix1]->Delete();
    m_imgMap[ix1]->Delete();
    m_actor2D[ix1]->Delete();
    m_imgReslice[ix1]->Delete();
  }

  if(m_transFilter) m_transFilter->Delete();
  if(m_rayMapper) m_rayMapper->Delete();
  if(m_volumeActor) m_volumeActor->Delete();

  if(m_outline) m_outline->Delete();
  if(m_outlineMapper) m_outlineMapper->Delete();
  if(m_outlineActor) m_outlineActor->Delete();

}


//! Take info from the data object. 
void rt3DVolumeRenderObject::update() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);
  if (!m_isInit || !dObj || !dObj->isDataValid()) return;

  double range[2];
  int dims[3];
  for (int ix1=0; ix1<3; ix1++) {
    if (dObj->getInterpolation() == 0) {
      m_imgReslice[ix1]->SetInterpolationModeToNearestNeighbor();
    } else if (dObj->getInterpolation() == 1) {
      m_imgReslice[ix1]->SetInterpolationModeToLinear();
    } else if (dObj->getInterpolation() == 2) {
      m_imgReslice[ix1]->SetInterpolationModeToCubic();
    }

    m_imgCast[ix1]->Update();
    m_imgCast[ix1]->GetOutput()->GetScalarRange(range);
    m_imgCast[ix1]->GetOutput()->GetDimensions(dims);

    m_imgMap[ix1]->SetColorWindow( dObj->getWindow() );
    m_imgMap[ix1]->SetColorLevel( dObj->getLevel() );

    m_texturePlane[ix1].setWindow(dObj->getWindow());
    m_texturePlane[ix1].setLevel(dObj->getLevel());

    m_actor2D[ix1]->SetPosition(0, 0);
    m_actor2D[ix1]->SetPosition2(1, 1);

    // Fix the extents
    int extents[4];
    extents[0] = 0;
    extents[1] = dims[0];
    extents[2] = 0;
    extents[3] = dims[1];
    m_imgMap[ix1]->UseCustomExtentsOn();
    m_imgMap[ix1]->SetCustomDisplayExtents(extents);
  }

  update3PlaneStatus();

  if (dObj->getRenderRayTraceVolume()) {
    m_volumeActor->SetVisibility(1);

    // Update the ray cast function
    m_rayMapper->SetVolumeRayCastFunction( dObj->getRayCastFunction() );
  } else {
    m_volumeActor->SetVisibility(0);
  }

  if ( m_mainWin ) {
    m_mainWin->setRenderFlag3D(true);
  }
}

void rt3DVolumeRenderObject::newDataAvailable() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);
  if (!dObj || !dObj->isDataValid()) return;

  if (dObj->getInterpolation() == 0) {
    m_transFilter->SetInterpolationModeToNearestNeighbor();
  } else if (dObj->getInterpolation() == 1) {
    m_transFilter->SetInterpolationModeToLinear();
  } else if (dObj->getInterpolation() == 2) {
    m_transFilter->SetInterpolationModeToCubic();
  }

  // Transform the data before it enters the pipeline.
  m_transFilter->SetOutputOriginToDefault();
  m_transFilter->SetOutputSpacingToDefault();
  m_transFilter->SetOutputExtentToDefault();
  m_transFilter->AutoCropOutputOn();
  m_transFilter->AddInput( dObj->getUShortData() );
  m_transFilter->SetResliceAxes( dObj->getTransform()->GetMatrix() );
  m_transFilter->Update();

  m_transFilter->GetOutput()->Update();

  for (int ix1=0; ix1<3; ix1++) {
    m_imgReslice[ix1]->SetOutputOriginToDefault();
    m_imgReslice[ix1]->SetOutputSpacingToDefault();
    m_imgReslice[ix1]->SetOutputExtentToDefault();
    m_imgReslice[ix1]->AutoCropOutputOff();
    m_imgReslice[ix1]->SetOutputDimensionality(2);
    m_imgReslice[ix1]->AddInput( m_transFilter->GetOutput() );
  }

  for (int ix1=0; ix1<3; ix1++) {
    m_texturePlane[ix1].setImageData(m_imgReslice[ix1]->GetOutput());
  }

  // The reset calls will lead to three modified calls on the data object and then to an update call.
  resetAxialPlane();
  resetSagittalPlane();
  resetCoronalPlane();

  m_isInit = true;
}

void rt3DVolumeRenderObject::resetAxialPlane() {
  double bounds[6];
  m_transFilter->GetOutput()->GetBounds(bounds);

  double pts[4][3];

  pts[0][0] = bounds[0];
  pts[0][1] = bounds[2];
  pts[0][2] = (bounds[4]+bounds[5])/2.0f;

  pts[1][0] = bounds[0];
  pts[1][1] = bounds[3];
  pts[1][2] = (bounds[4]+bounds[5])/2.0f;

  pts[2][0] = bounds[1];
  pts[2][1] = bounds[3];
  pts[2][2] = (bounds[4]+bounds[5])/2.0f;

  pts[3][0] = bounds[1];
  pts[3][1] = bounds[2];
  pts[3][2] = (bounds[4]+bounds[5])/2.0f;

  m_boxOutline[0].setCorners(pts[0], pts[1], pts[2], pts[3]);
  m_texturePlane[0].setCorners(pts[0], pts[1], pts[3]);
  adjustReslice(0);
  m_planeControl[0].setTransform(m_boxOutline[0].getTransform());
  m_planeControl[0].setSize(bounds[3]-bounds[2], bounds[1]-bounds[0] );

  // Modify the data object so that the update function will be called.
  m_dataObj->Modified();
}

void rt3DVolumeRenderObject::resetSagittalPlane() {
  double bounds[6];
  m_transFilter->GetOutput()->GetBounds(bounds);

  double pts[4][3];

  pts[0][0] = (bounds[0]+bounds[1])/2.0f;
  pts[0][1] = bounds[2];
  pts[0][2] = bounds[4];

  pts[1][0] = (bounds[0]+bounds[1])/2.0f;
  pts[1][1] = bounds[3];
  pts[1][2] = bounds[4];

  pts[2][0] = (bounds[0]+bounds[1])/2.0f;
  pts[2][1] = bounds[3];
  pts[2][2] = bounds[5];

  pts[3][0] = (bounds[0]+bounds[1])/2.0f;
  pts[3][1] = bounds[2];
  pts[3][2] = bounds[5];

  m_boxOutline[1].setCorners(pts[0], pts[1], pts[2], pts[3]);
  m_texturePlane[1].setCorners(pts[0], pts[1], pts[3]);
  adjustReslice(1);
  m_planeControl[1].setTransform(m_boxOutline[1].getTransform());
  m_planeControl[1].setSize(bounds[3]-bounds[2], bounds[5]-bounds[4] );

  // Modify the data object so that the update function will be called.
  m_dataObj->Modified();
}

void rt3DVolumeRenderObject::resetCoronalPlane() {
  double bounds[6];
  m_transFilter->GetOutput()->GetBounds(bounds);

  double pts[4][3];

  pts[0][0] = bounds[0];
  pts[0][1] = (bounds[2]+bounds[3])/2.0f;
  pts[0][2] = bounds[4];

  pts[1][0] = bounds[0];
  pts[1][1] = (bounds[2]+bounds[3])/2.0f;
  pts[1][2] = bounds[5];

  pts[2][0] = bounds[1];
  pts[2][1] = (bounds[2]+bounds[3])/2.0f;
  pts[2][2] = bounds[5];

  pts[3][0] = bounds[1];
  pts[3][1] = (bounds[2]+bounds[3])/2.0f;
  pts[3][2] = bounds[4];

  m_boxOutline[2].setCorners(pts[0], pts[1], pts[2], pts[3]);
  m_texturePlane[2].setCorners(pts[0], pts[1], pts[3]);
  adjustReslice(2);
  m_planeControl[2].setTransform(m_boxOutline[2].getTransform());
  m_planeControl[2].setSize(bounds[5]-bounds[4], bounds[1]-bounds[0] );

  // Modify the data object so that the update function will be called.
  m_dataObj->Modified();
}




void rt3DVolumeRenderObject::setRenderQuality(double quality) {
  if (!m_mainWin) return;
  double qual;

  if (quality > 1.0f) qual = 1.0f;
  else if (quality <= 0.0f) qual = 0.01f;
  else qual = quality;

  m_rayMapper->SetImageSampleDistance(11.0f - 10.0f*qual);
}

//! Add this object to the given renderer.
bool rt3DVolumeRenderObject::addToRenderer(vtkRenderer* ren) {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);
  if (!ren || !dObj || !dObj->isDataValid()) return false;
  setVisible3D(true);
  if (!ren->HasViewProp(m_volumeActor)) {
    ren->AddViewProp(m_volumeActor);
  }
  if (!ren->HasViewProp(m_outlineActor)) {
    ren->AddViewProp(m_outlineActor);
  }
  for (int ix1=0; ix1<3; ix1++) {
    ren->AddViewProp(m_boxOutline[ix1].getActor());
    ren->AddViewProp(m_texturePlane[ix1].getActor());
  }
  update3PlaneStatus();

  customQVTKWidget* renWid;
  renWid = rtObjectManager::instance().getMainWinHandle()->getRenderWidget();
  // Connect mouse actions
  connect(renWid, SIGNAL(interMousePress(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
  connect(renWid, SIGNAL(interKeyPress(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
  connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
  connect(renWid, SIGNAL(interWheel(QWheelEvent*)), this, SLOT(wheelEvent(QWheelEvent*)));

  return true;
}

//! Remove this object from the given renderer.
bool rt3DVolumeRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
  if (ren->HasViewProp(m_volumeActor)) {
    ren->RemoveViewProp(m_volumeActor);
  }
  if (ren->HasViewProp(m_outlineActor)) {
    ren->RemoveViewProp(m_outlineActor);
  }
  for (int ix1=0; ix1<3; ix1++) {
    ren->RemoveViewProp(m_boxOutline[ix1].getActor());
    ren->RemoveViewProp(m_texturePlane[ix1].getActor());
  }
  update3PlaneStatus();

  customQVTKWidget* renWid;
  renWid = rtObjectManager::instance().getMainWinHandle()->getRenderWidget();

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*)), this, SLOT(wheelEvent(QWheelEvent*)));

  return true;
}

void rt3DVolumeRenderObject::update3PlaneStatus() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  if (!dObj) return;

  if ( dObj->getAxial3D() && getVisible3D() ) {
    m_boxOutline[0].getActor()->SetVisibility(1);
    m_texturePlane[0].getActor()->SetVisibility(1);
  } else {
    m_boxOutline[0].getActor()->SetVisibility(0);
    m_texturePlane[0].getActor()->SetVisibility(0);
  }

  if ( dObj->getSagittal3D() && getVisible3D() ) {
    m_boxOutline[1].getActor()->SetVisibility(1);
    m_texturePlane[1].getActor()->SetVisibility(1);
  } else {
    m_boxOutline[1].getActor()->SetVisibility(0);
    m_texturePlane[1].getActor()->SetVisibility(0);
  }

  if ( dObj->getCoronal3D() && getVisible3D() ) {
    m_boxOutline[2].getActor()->SetVisibility(1);
    m_texturePlane[2].getActor()->SetVisibility(1);
  } else {
    m_boxOutline[2].getActor()->SetVisibility(0);
    m_texturePlane[2].getActor()->SetVisibility(0);
  }
}

//! Create the correct data object.
void rt3DVolumeRenderObject::setupDataObject() {
  rt3DVolumeDataObject *temp = new rt3DVolumeDataObject();
  m_dataObj = temp;
  connect(temp, SIGNAL(newImageData()), this, SLOT(newDataAvailable()));
  connect(temp, SIGNAL(axialResetSignal()), this, SLOT(resetAxialPlane()));
  connect(temp, SIGNAL(sagittalResetSignal()), this, SLOT(resetSagittalPlane()));
  connect(temp, SIGNAL(coronalResetSignal()), this, SLOT(resetCoronalPlane()));
}


//! Create the part of the pipeline that is done first. 
void rt3DVolumeRenderObject::setupPipeline() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  m_transFilter = vtkImageReslice::New();
  m_rayMapper = vtkVolumeRayCastMapper::New();
  m_volumeActor = vtkVolume::New();

  m_outline = vtkOutlineFilter::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();

  // Volume Rendering
  m_rayMapper->SetInput( m_transFilter->GetOutput() );
  m_rayMapper->SetVolumeRayCastFunction( dObj->getRayCastFunction() );

  // Manually adjust the sample distances
  m_rayMapper->AutoAdjustSampleDistancesOff ();

  m_volumeActor->SetMapper(m_rayMapper);
  m_volumeActor->SetProperty( dObj->getVolumeProperty() );
  m_volumeActor->SetVisibility(0);

  m_outline->SetInput( m_transFilter->GetOutput()  );
  m_outlineMapper->SetInput( m_outline->GetOutput() );
  m_outlineActor->SetMapper( m_outlineMapper );

  //m_outlineMapper->GlobalImmediateModeRenderingOn();

  m_pipe3D.push_back(m_volumeActor);
  m_pipe3D.push_back(m_outlineActor);

  // Cut planes
  m_boxOutline[0].setOutlineColor(0, 1, 0);
  m_boxOutline[1].setOutlineColor(1, 1, 0);
  m_boxOutline[2].setOutlineColor(1, 0, 1);
  for (int ix1=0; ix1<3; ix1++) {
    m_pipe3D.push_back(m_boxOutline[ix1].getActor());
    m_pipe3D.push_back(m_texturePlane[ix1].getActor());
  }

  // Do the 2D planes as well.
  for (int ix1=0; ix1<3; ix1++) {
    m_imgCast[ix1] = vtkImageCast::New();
    m_imgMap[ix1] = vtkImageMapper::New();
    m_actor2D[ix1] = vtkActor2D::New();

    m_imgCast[ix1]->SetInput(m_imgReslice[ix1]->GetOutput());

    m_imgCast[ix1]->SetOutputScalarTypeToUnsignedShort();

    m_imgMap[ix1]->SetInputConnection(m_imgCast[ix1]->GetOutputPort());
    m_imgMap[ix1]->RenderToRectangleOn();

    m_actor2D[ix1]->SetMapper(m_imgMap[ix1]);
  }
  m_pipe2D.insert("Axial", m_actor2D[0]);
  m_pipe2D.insert("Sagittal", m_actor2D[1]);
  m_pipe2D.insert("Coronal", m_actor2D[2]);

  // Update the volume
  tryUpdate();
}


//! The position of the center of the plane is given
bool rt3DVolumeRenderObject::getObjectLocation(double loc[6]) {
  if (!m_volumeActor) return false;

  m_volumeActor->GetBounds(loc);

  return true;
}

void rt3DVolumeRenderObject::mousePressEvent(QMouseEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;

  if (m_planeControl[m_currentPlane].isShowing()) {
    m_planeControl[m_currentPlane].mousePressEvent(event);
    if ( m_mainWin ) m_mainWin->setRenderFlag3D(true);
  }
}

void rt3DVolumeRenderObject::mouseMoveEvent(QMouseEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;
  if (m_planeControl[m_currentPlane].isShowing()) {
    m_planeControl[m_currentPlane].mouseMoveEvent(event);
    if ( m_mainWin ) m_mainWin->setRenderFlag3D(true);
  }
}

void rt3DVolumeRenderObject::mouseReleaseEvent(QMouseEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;

  if (m_planeControl[m_currentPlane].isShowing()) {
    vtkTransform *t = vtkTransform::New();
    m_planeControl[m_currentPlane].mouseReleaseEvent(event);
    m_planeControl[m_currentPlane].getTransform(t);

    m_boxOutline[m_currentPlane].setTransform(t);
    m_boxOutline[m_currentPlane].update();
    m_texturePlane[m_currentPlane].setTransform(t);
    m_texturePlane[m_currentPlane].update();
    adjustReslice(m_currentPlane);

    t->Delete();

    // Modify the data object so that the update function will be called.
    m_dataObj->Modified();
  }
}

void rt3DVolumeRenderObject::mouseDoubleClickEvent(QMouseEvent* event) {
  vtkProp* temp;

  temp = rtObjectManager::instance().getMainWinHandle()->getSelectedProp();
  m_selectedProp = NULL;
  m_currentPlane = -1;
  for (int ix1=0; ix1<3; ix1++) {
    if (m_planeControl[ix1].isShowing())
      m_planeControl[ix1].hide();

    if (temp) {
      if (temp == m_texturePlane[ix1].getActor() || temp == m_boxOutline[ix1].getActor()) {
        m_selectedProp = temp;
        m_planeControl[ix1].show();
        m_currentPlane = ix1;
      }
    }
  }
  if ( m_mainWin ) m_mainWin->setRenderFlag3D(true);


}

void rt3DVolumeRenderObject::keyPressEvent(QKeyEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;
}

void rt3DVolumeRenderObject::keyReleaseEvent(QKeyEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;
}

void rt3DVolumeRenderObject::wheelEvent(QWheelEvent* event) {
  if (!m_selectedProp || m_currentPlane == -1) return;
  if (m_planeControl[m_currentPlane].isShowing()) {
    vtkTransform *t = vtkTransform::New();
    m_planeControl[m_currentPlane].wheelEvent(event);
    m_planeControl[m_currentPlane].getTransform(t);

    m_boxOutline[m_currentPlane].setTransform(t);
    m_boxOutline[m_currentPlane].update();
    m_texturePlane[m_currentPlane].setTransform(t);
    m_texturePlane[m_currentPlane].update();
    adjustReslice(m_currentPlane);

    t->Delete();

    if ( m_mainWin ) m_mainWin->setRenderFlag3D(true);
  }
}

////////////
// Protected Functions
/////////////
void rt3DVolumeRenderObject::adjustReslice(int slice) {
  vtkTransform *mat = vtkTransform::New();

  mat->Concatenate(m_boxOutline[slice].getTransform()->GetMatrix());

  double spacing[3];
  double planeAxis1[3];
  double planeAxis2[3];

  m_transFilter->GetOutput()->GetSpacing(spacing);

  m_boxOutline[slice].getVector1(planeAxis1);
  m_boxOutline[slice].getVector2(planeAxis2);

  double planeSizeX = vtkMath::Normalize(planeAxis1);
  double planeSizeY = vtkMath::Normalize(planeAxis2);

  double spacingX = fabs(planeAxis1[0]*spacing[0])+
                   fabs(planeAxis1[1]*spacing[1])+
                   fabs(planeAxis1[2]*spacing[2]);

  double spacingY = fabs(planeAxis2[0]*spacing[0])+
                   fabs(planeAxis2[1]*spacing[1])+
                   fabs(planeAxis2[2]*spacing[2]);

  // Reslice
  m_imgReslice[slice]->SetResliceAxes(mat->GetMatrix());

  // Fix the output origin, spacing, extent
  m_imgReslice[slice]->SetOutputOrigin(0.5*spacingX, 0.5*spacingY, 0);
  m_imgReslice[slice]->SetOutputSpacing(spacingX, spacingY, 1);
  m_imgReslice[slice]->SetOutputExtent(0, planeSizeX/fabs(spacingX), 0, planeSizeY/fabs(spacingY), 0, 0);

  mat->Delete();
}
