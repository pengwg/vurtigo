#ifndef RTARROWPIPELINE_H
#define RTARROWPIPELINE_H

// VTK
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

// Local
#include "rtTubePipeline.h"

//! Pipeline for a simple arrow in 3D.
/*!
  An arrow is just a tube with a cone on the front to indicate the direction the arrow is pointing in.
  */
class rtArrowPipeline : public rtTubePipeline
{
public:
  rtArrowPipeline();
  ~rtArrowPipeline();

  inline vtkActor* getTipActor() { return m_coneActor; }
  inline vtkProperty* getTipProperty() { return m_coneActor->GetProperty(); }

protected:
  vtkConeSource* m_coneSource;
  vtkPolyDataMapper* m_coneMapper;
  vtkActor* m_coneActor;
};

#endif // RTARROWPIPELINE_H
