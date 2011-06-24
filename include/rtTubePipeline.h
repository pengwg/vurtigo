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
#ifndef RTTUBEPIPELINE_H
#define RTTUBEPIPELINE_H

// STD C++
#include <vector>

// VTK
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>

class rtTubePipeline
{
public:
  rtTubePipeline();
  ~rtTubePipeline();

  inline vtkActor* const getTubeActor() { return m_tubeActor; }

  inline vtkProperty* const getTubeProperty() { return m_tubeActor->GetProperty(); }

  inline void clearPointList() { m_tubePoints->Reset(); }

  void setNumPoints(int size);
  inline int getNumPoints() { return m_tubePoints->GetNumberOfPoints(); }

  inline void setPoint(int pos, double pt[3]) { m_tubePoints->SetPoint(pos, pt); }
  inline void setPoint(int pos, double ptx, double pty, double ptz) { m_tubePoints->SetPoint(pos, ptx, pty, ptz); }
  inline void getPoint(int pos, double pt[3]) { m_tubePoints->GetPoint(pos, pt); }

  inline void tubeVisibilityOn() { m_tubeActor->VisibilityOn(); }
  inline void tubeVisibilityOff() { m_tubeActor->VisibilityOff(); }
  inline bool tubeGetVisibility() { return m_tubeActor->GetVisibility(); }

  //! Set the render quality in the range [0,1]
  inline void setTubeRenderQuality(float quality) {
    if (quality < 0.0) quality = 0.0;
    else if (quality > 1.0) quality = 1.0;
    m_renderQuality = quality;
    m_tubeFilter->SetNumberOfSides( 3.0f+m_renderQuality*50.0f );
  }

  inline float getTubeRenderQuality() { return m_renderQuality; }

  inline void setTubeThickness(double t) { m_tubeFilter->SetRadius(t); }
  inline double getTubeThickness() { return m_tubeFilter->GetRadius(); }

protected:
  vtkPoints* m_tubePoints;
  vtkCellArray* m_tubeCellArray;
  vtkPolyData* m_tubePolyData;
  vtkTubeFilter* m_tubeFilter;
  vtkPolyDataMapper* m_tubeMapper;
  vtkActor* m_tubeActor;

  float m_renderQuality;
};

#endif // RTTUBEPIPELINE_H
