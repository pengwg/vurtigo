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
