#ifndef RTSINGLE3DPOINTPIPELINE_H
#define RTSINGLE3DPOINTPIPELINE_H

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

//! Complete rendering pipeline for a single point in 3D space.
class rtSingle3DPointPipeline
{
public:
  rtSingle3DPointPipeline();
  ~rtSingle3DPointPipeline();

  inline vtkActor* getActor() { return m_actor; }

  void setPosition(double x, double y, double z);

  //! Set the radius of the displayed point.
  void setRadius(double r);

  void setResolution(int resolution);

  void setProperty(vtkProperty* prop);

protected:
  // VTK pipeline objects.
  vtkSphereSource* m_sphere;
  vtkPolyDataMapper* m_mapper;
  vtkActor* m_actor;
};

#endif // RTSINGLE3DPOINTPIPELINE_H
