#include "rtTubePipeline.h"

rtTubePipeline::rtTubePipeline()
{ 
  // Create the vtk objects
  m_tubePoints = vtkPoints::New();
  m_tubeCellArray = vtkCellArray::New();
  m_tubePolyData = vtkPolyData::New();
  m_tubeFilter = vtkTubeFilter::New();
  m_tubeMapper = vtkPolyDataMapper::New();
  m_tubeActor = vtkActor::New();


  // Build the pipeline.
  m_tubePoints->Reset();
  m_tubePoints->SetDataTypeToDouble ();
  m_tubePoints->SetNumberOfPoints(2);
  m_tubePoints->SetPoint(0, 0.0, 0.0, 0.0);
  m_tubePoints->SetPoint(1, 1.0, 1.0, 1.0);

  m_tubeCellArray->InsertNextCell(2);
  m_tubeCellArray->InsertCellPoint(0);
  m_tubeCellArray->InsertCellPoint(1);

  m_tubePolyData->SetPoints(m_tubePoints);
  m_tubePolyData->SetLines(m_tubeCellArray);

  m_tubeFilter->SetInput(m_tubePolyData);
  m_tubeFilter->SetNumberOfSides(25);

  m_tubeMapper->SetInput(m_tubeFilter->GetOutput());

  m_tubeActor->SetMapper(m_tubeMapper);

  setTubeRenderQuality(0.5);
}

rtTubePipeline::~rtTubePipeline() {
  m_tubePoints->Delete();
  m_tubeCellArray->Delete();
  m_tubePolyData->Delete();
  m_tubeFilter->Delete();
  m_tubeMapper->Delete();
  m_tubeActor->Delete();
}

void rtTubePipeline::setNumPoints(int size) {
  m_tubePoints->SetNumberOfPoints(size);

  // Setup the cell array
  m_tubeCellArray->Reset();
  m_tubeCellArray->InsertNextCell(size);
  for (int ix1=0; ix1<size; ix1++) {
    m_tubeCellArray->InsertCellPoint(ix1);
  }
  m_tubePolyData->Modified();
}
