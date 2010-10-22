#include "rtArrowPipeline.h"

rtArrowPipeline::rtArrowPipeline()
{
  m_coneSource = vtkConeSource::New();
  m_coneMapper = vtkPolyDataMapper::New();
  m_coneActor = vtkActor::New();

  m_coneMapper->SetInput(m_coneSource->GetOutput());
  m_coneActor->SetMapper(m_coneMapper);
}

rtArrowPipeline::~rtArrowPipeline()
{
  m_coneSource->Delete();
  m_coneMapper->Delete();
  m_coneActor->Delete();
}
