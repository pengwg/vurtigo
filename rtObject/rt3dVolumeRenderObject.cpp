#include "rt3dVolumeRenderObject.h"
#include "rt3dVolumeDataObject.h"

rt3DVolumeRenderObject::rt3DVolumeRenderObject() {
  setObjectType(rtConstants::OT_3DObject);
  setName("3D Volume Renderer");
  setupDataObject();
  setupPipeline();
}


rt3DVolumeRenderObject::~rt3DVolumeRenderObject() {
  if(m_tranformFilter) m_tranformFilter->Delete();
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
}

//! Create the correct data object.
void rt3DVolumeRenderObject::setupDataObject() {
  m_dataObj = new rt3DVolumeDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt3DVolumeRenderObject::setupPipeline() {
  rt3DVolumeDataObject* dObj = static_cast<rt3DVolumeDataObject*>(m_dataObj);

  m_tranformFilter = vtkTransformFilter::New();
  m_rayMapper = vtkVolumeRayCastMapper::New();
  m_volumeActor = vtkVolume::New();

  m_outline = vtkOutlineFilter::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();

  // Volume Rendering
  m_tranformFilter->SetTransform( dObj->getTransform() );
  m_tranformFilter->AddInput( dObj->getImageData() );
  m_rayMapper->SetInput( m_tranformFilter->GetOutput() );
  m_rayMapper->SetVolumeRayCastFunction( dObj->getRayCastFunction() );
  m_volumeActor->SetMapper(m_rayMapper);
  m_volumeActor->SetProperty( dObj->getVolumeProperty() );

  m_outline->SetInput( m_tranformFilter->GetOutput() );
  m_outlineMapper->SetInput( m_outline->GetOutput() );
  m_outlineActor->SetMapper( m_outlineMapper );

  m_pipe3D->AddPart(m_volumeActor);
  m_pipe3D->AddPart(m_outlineActor);

}
