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
  inline void setNumPoints(int size) { m_tubePoints->SetNumberOfPoints(size); }
  inline int getNumPoints() { return m_tubePoints->GetNumberOfPoints(); }

  inline void setPoint(int pos, double pt[3]) { m_tubePoints->SetPoint(pos, pt); }
  inline void getPoint(int pos, double pt[3]) { m_tubePoints->GetPoint(pos, pt); }

protected:
  vtkPoints* m_tubePoints;
  vtkCellArray* m_tubeCellArray;
  vtkPolyData* m_tubePolyData;
  vtkTubeFilter* m_tubeFilter;
  vtkPolyDataMapper* m_tubeMapper;
  vtkActor* m_tubeActor;
};

#endif // RTTUBEPIPELINE_H
