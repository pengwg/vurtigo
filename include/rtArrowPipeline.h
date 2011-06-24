/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
