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
  Note that this pipeline is actually made up of two components.
  */
class rtArrowPipeline : public rtTubePipeline
{
public:
  rtArrowPipeline();
  ~rtArrowPipeline();

  inline vtkActor* getConeActor() { return m_coneActor; }
  inline vtkProperty* getConeProperty() { return m_coneActor->GetProperty(); }

  inline void coneVisibilityOn() { m_coneActor->VisibilityOn(); }
  inline void coneVisibilityOff() { m_coneActor->VisibilityOff(); }
  inline bool coneGetVisibility() { return m_coneActor->GetVisibility(); }

  inline void setConeRadius(double r) { m_coneSource->SetRadius(r); }
  inline double getConeRadius() { return m_coneSource->GetRadius(); }

  inline void setConeHeight(double h) { m_coneSource->SetHeight(h); }
  inline double getConeHeight() { return m_coneSource->GetHeight(); }

  inline void setConeResolution(int res) { m_coneSource->SetResolution(res); }
  inline int getConeResolution() { return m_coneSource->GetResolution(); }

  inline void setConeCenter(double c[3]) { m_coneSource->SetCenter(c); }
  inline void setConeCenter(double cx, double cy, double cz) { m_coneSource->SetCenter(cx, cy, cz); }
  inline void getConeCenter(double c[3]) { m_coneSource->GetCenter(c); }

  inline void setConeDirection(double d[3]) { m_coneSource->SetDirection(d); }
  inline void setConeDirection(double dx, double dy, double dz) { m_coneSource->SetDirection(dx, dy, dz); }
  inline void getConeDirection(double d[3]) { m_coneSource->GetDirection(d); }

protected:
  vtkConeSource* m_coneSource;
  vtkPolyDataMapper* m_coneMapper;
  vtkActor* m_coneActor;
};

#endif // RTARROWPIPELINE_H
