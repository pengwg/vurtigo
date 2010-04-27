#include "rtSingle3DPointPipeline.h"

rtSingle3DPointPipeline::rtSingle3DPointPipeline() {
  m_sphere = vtkSphereSource::New();
  m_mapper = vtkPolyDataMapper::New();
  m_actor = vtkActor::New();

  m_mapper->SetInput(m_sphere->GetOutput());
  m_actor->SetMapper(m_mapper);
}

rtSingle3DPointPipeline::~rtSingle3DPointPipeline() {
  m_sphere->Delete();
  m_mapper->Delete();
  m_actor->Delete();
}

void rtSingle3DPointPipeline::setPosition(double x, double y, double z) {
  m_sphere->SetCenter(x,y,z);
}

void rtSingle3DPointPipeline::setRadius(double r) {
  if (r<=0.0) return;
  m_sphere->SetRadius(r);
}

void rtSingle3DPointPipeline::setResolution(int resolution) {
  if (resolution <= 0) return;

  m_sphere->SetThetaResolution(resolution);
  m_sphere->SetPhiResolution(resolution);
}

void rtSingle3DPointPipeline::setProperty(vtkProperty* prop) {
  if (!prop) return;
  m_actor->SetProperty(prop);
}
