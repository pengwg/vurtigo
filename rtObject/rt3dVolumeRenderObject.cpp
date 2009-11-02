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

#include <vtkProperty.h>

rt3DVolumeRenderObject::rt3DVolumeRenderObject() {
  setObjectType(rtConstants::OT_3DObject);
  setName("3D Volume Renderer");

  m_planes[0] = new rtImagePlaneWidget();
  m_planes[1] = new rtImagePlaneWidget();
  m_planes[2] = new rtImagePlaneWidget();

  m_firstInit = true;

  setupDataObject();
  setupPipeline();
}


rt3DVolumeRenderObject::~rt3DVolumeRenderObject() {

  for (int ix1=0; ix1<3; ix1++) {
    delete m_planes[ix1];
    m_imgCast[ix1]->Delete();
    m_imgMap[ix1]->Delete();
    m_actor2D[ix1]->Delete();
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
  if (!dObj || !dObj->isDataValid()) return;

  if (m_firstInit) {
    int slicePos[3];
    // Save the positions.
    for (int ix1=0; ix1<3; ix1++) {
      if (m_planes[ix1]->GetPlaneOrientation() >= 0 && m_planes[ix1]->GetPlaneOrientation() <= 2) {
        slicePos[ix1] = m_planes[ix1]->GetSliceIndex();
      } else {
        // Reset the slice.
        slicePos[ix1] = 0;
      }
    }

    if (dObj->getInterpolation() == 0) {
      m_transFilter->SetInterpolationModeToNearestNeighbor();
    } else if (dObj->getInterpolation() == 1) {
      m_transFilter->SetInterpolationModeToLinear();
    } else if (dObj->getInterpolation() == 2) {
      m_transFilter->SetInterpolationModeToCubic();
    }
    m_transFilter->SetResliceAxes( dObj->getTransform()->GetMatrix() );
    m_transFilter->Update();

    for (int ix1=0; ix1<3; ix1++) {
      m_planes[ix1]->SetInput( m_transFilter->GetOutput() );

    }

    m_planes[0]->SetPlaneOrientationToXAxes();
    m_planes[1]->SetPlaneOrientationToYAxes();
    m_planes[2]->SetPlaneOrientationToZAxes();

    m_planes[0]->SetPlaneOrientationToZAxes();
    m_planes[1]->SetPlaneOrientationToXAxes();
    m_planes[2]->SetPlaneOrientationToYAxes();

    // Restore the slice position
    for (int ix1=0; ix1<3; ix1++) {
      m_planes[ix1]->SetSliceIndex(slicePos[ix1]);
    }
    m_firstInit = false;
  }

  double range[2];
  int dims[3];
  for (int ix1=0; ix1<3; ix1++) {

    if (dObj->getInterpolation() == 0) {
      m_planes[ix1]->SetResliceInterpolateToNearestNeighbour();
    } else if (dObj->getInterpolation() == 1) {
      m_planes[ix1]->SetResliceInterpolateToLinear();
    } else if (dObj->getInterpolation() == 2) {
      m_planes[ix1]->SetResliceInterpolateToCubic();
    }

    m_imgCast[ix1]->Update();
    m_imgCast[ix1]->GetOutput()->GetScalarRange(range);
    m_imgCast[ix1]->GetOutput()->GetDimensions(dims);

    m_imgMap[ix1]->SetColorWindow( range[1]-range[0] );
    m_imgMap[ix1]->SetColorLevel( (range[0]+range[1])/2.0f );

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

    m_planes[ix1]->UpdatePlacement();
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
    if (!ren->HasViewProp( m_planes[ix1]->getMarginActor() ) ) {
      ren->AddViewProp( m_planes[ix1]->getMarginActor() );
    }
    if (!ren->HasViewProp( m_planes[ix1]->getPlaneOutlineActor() ) ) {
      ren->AddViewProp( m_planes[ix1]->getPlaneOutlineActor() );
    }
    if (!ren->HasViewProp( m_planes[ix1]->getTexturePlaneActor() ) ) {
      ren->AddViewProp( m_planes[ix1]->getTexturePlaneActor() );
    }

  }
  update3PlaneStatus();
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
    if (ren->HasViewProp( m_planes[ix1]->getMarginActor() ) ) {
      ren->RemoveViewProp( m_planes[ix1]->getMarginActor() );
    }
    if (ren->HasViewProp( m_planes[ix1]->getPlaneOutlineActor() ) ) {
      ren->RemoveViewProp( m_planes[ix1]->getPlaneOutlineActor() );
    }
    if (ren->HasViewProp( m_planes[ix1]->getTexturePlaneActor() ) ) {
      ren->RemoveViewProp( m_planes[ix1]->getTexturePlaneActor() );
    }
  }
  update3PlaneStatus();
  return true;
}

void rt3DVolumeRenderObject::update3PlaneStatus() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  if (!dObj) return;

  if ( dObj->getAxial3D() && getVisible3D() ) {
    m_planes[0]->On();
  } else {
    m_planes[0]->Off();
  }

  if ( dObj->getSagittal3D() && getVisible3D() ) {
    m_planes[1]->On();
  } else {
    m_planes[1]->Off();
  }

  if ( dObj->getCoronal3D() && getVisible3D() ) {
    m_planes[2]->On();
  } else {
    m_planes[2]->Off();
  }
}

//! Create the correct data object.
void rt3DVolumeRenderObject::setupDataObject() {
  m_dataObj = new rt3DVolumeDataObject();
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

  m_firstInit = true;

  // Transform the data before it enters the pipeline.
  m_transFilter->SetOutputOriginToDefault();
  m_transFilter->SetOutputSpacingToDefault();
  m_transFilter->SetOutputExtentToDefault();
  m_transFilter->AutoCropOutputOn();
  m_transFilter->AddInput( dObj->getUShortData() );

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

  // Add the cut-planes
  m_planes[0]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[0]->RestrictPlaneToVolumeOn();
  m_planes[0]->GetPlaneProperty()->SetColor(1,0,0);
  m_planes[0]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[0]->fillWidgetActors(&m_pipe3D);

  m_planes[1]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[1]->RestrictPlaneToVolumeOn();
  m_planes[1]->GetPlaneProperty()->SetColor(1,1,0);
  m_planes[1]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[1]->fillWidgetActors(&m_pipe3D);

  m_planes[2]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[2]->RestrictPlaneToVolumeOn();
  m_planes[2]->GetPlaneProperty()->SetColor(1,0,1);
  m_planes[2]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[2]->fillWidgetActors(&m_pipe3D);

  // Do the 2D planes as well.
  for (int ix1=0; ix1<3; ix1++) {
    m_imgCast[ix1] = vtkImageCast::New();
    m_imgMap[ix1] = vtkImageMapper::New();
    m_actor2D[ix1] = vtkActor2D::New();

    m_imgCast[ix1]->SetInput(m_planes[ix1]->GetResliceOutput());
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
