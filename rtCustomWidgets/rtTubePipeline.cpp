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
