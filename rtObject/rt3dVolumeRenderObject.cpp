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

  setupDataObject();
  setupPipeline();
}


rt3DVolumeRenderObject::~rt3DVolumeRenderObject() {

  delete m_planes[0];
  delete m_planes[1];
  delete m_planes[2];

  if(m_rayMapper) m_rayMapper->Delete();
  if(m_volumeActor) m_volumeActor->Delete();

  if(m_outline) m_outline->Delete();
  if(m_outlineMapper) m_outlineMapper->Delete();
  if(m_outlineActor) m_outlineActor->Delete();
}


//! Take info from the data object. 
void rt3DVolumeRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();

  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  if (dObj->isDataValid()) {
    m_planes[0]->SetInput( dObj->getUShortData() );
    m_planes[1]->SetInput( dObj->getUShortData() );
    m_planes[2]->SetInput( dObj->getUShortData() );

    m_planes[0]->SetPlaneOrientationToZAxes();
    m_planes[1]->SetPlaneOrientationToXAxes();
    m_planes[2]->SetPlaneOrientationToYAxes();
  }

  if ( dObj->getAxial3D() ) {
    m_planes[0]->On();
  } else {
    m_planes[0]->Off();
  }

  if ( dObj->getSagittal3D() ) {
    m_planes[1]->On();
  } else {
    m_planes[1]->Off();
  }

  if ( dObj->getCoronal3D() ) {
    m_planes[2]->On();
  } else {
    m_planes[2]->Off();
  }

  if (dObj->getRenderRayTraceVolume()) {
    m_volumeActor->SetVisibility(1);

    // Update the ray cast function
    m_rayMapper->SetVolumeRayCastFunction( dObj->getRayCastFunction() );
  } else {
    m_volumeActor->SetVisibility(0);
  }

  if (m_mainWin && m_pipe3D->GetNumberOfConsumers() > 0) {
    m_mainWin->setRenderFlag3D(true);
  }
}

//! Create the correct data object.
void rt3DVolumeRenderObject::setupDataObject() {
  m_dataObj = new rt3DVolumeDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt3DVolumeRenderObject::setupPipeline() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  m_rayMapper = vtkVolumeRayCastMapper::New();
  m_volumeActor = vtkVolume::New();

  m_outline = vtkOutlineFilter::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();

  // Volume Rendering
  m_rayMapper->SetInput( dObj->getUShortData() );
  m_rayMapper->SetVolumeRayCastFunction( dObj->getRayCastFunction() );

  m_volumeActor->SetMapper(m_rayMapper);
  m_volumeActor->SetProperty( dObj->getVolumeProperty() );
  m_volumeActor->SetUserTransform( dObj->getTransform() );
  m_volumeActor->SetVisibility(0);

  m_outline->SetInput( dObj->getUShortData()  );
  m_outlineMapper->SetInput( m_outline->GetOutput() );
  m_outlineActor->SetMapper( m_outlineMapper );
  m_outlineActor->SetUserTransform( dObj->getTransform() );

  m_pipe3D->AddPart(m_volumeActor);
  m_pipe3D->AddPart(m_outlineActor);

  // Add the cut-planes
  m_planes[0]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[0]->RestrictPlaneToVolumeOn();
  m_planes[0]->GetPlaneProperty()->SetColor(1,0,0);
  m_planes[0]->SetResliceInterpolateToNearestNeighbour();
  m_planes[0]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[0]->fillWidgetActors(m_pipe3D);

  m_planes[1]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[1]->RestrictPlaneToVolumeOn();
  m_planes[1]->GetPlaneProperty()->SetColor(1,1,0);
  m_planes[1]->SetResliceInterpolateToNearestNeighbour();
  m_planes[1]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[1]->fillWidgetActors(m_pipe3D);

  m_planes[2]->SetInteractor( rtObjectManager::instance().getMainWinHandle()->getInteractor() );
  m_planes[2]->RestrictPlaneToVolumeOn();
  m_planes[2]->GetPlaneProperty()->SetColor(1,0,1);
  m_planes[2]->SetResliceInterpolateToNearestNeighbour();
  m_planes[2]->SetPicker( rtObjectManager::instance().getMainWinHandle()->getGlobalCellPicker() );
  m_planes[2]->fillWidgetActors(m_pipe3D);

  // Update the volume
  update();
}
