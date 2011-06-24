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
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkMath.h>

#include "rtCrosshair2D.h"

rtCrosshair2D::rtCrosshair2D() {
  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(5);
  pts->SetPoint(0, 0.0, 0.5, 0.0);
  pts->SetPoint(1, 0.5, 1.0, 0.0);
  pts->SetPoint(2, 1.0, 0.5, 0.0);
  pts->SetPoint(3, 0.5, 0.0, 0.0);
  pts->SetPoint(4, 0.5, 0.5, 0.0);

  m_polyData->SetPoints(pts);
  pts->Delete();

  vtkCellArray* lines = vtkCellArray::New();
  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(4);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(4);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(2);
  lines->InsertCellPoint(4);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(3);
  lines->InsertCellPoint(4);
  m_polyData->SetLines(lines);
  lines->Delete();
}

rtCrosshair2D::~rtCrosshair2D() {
}

void rtCrosshair2D::update() {
  double temp[3];

  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(5);
  findMidpoint(m_origin, m_pt1, temp);
  pts->SetPoint(0, temp);
  findMidpoint(m_opposite, m_pt1, temp);
  pts->SetPoint(1, temp);
  findMidpoint(m_opposite, m_pt2, temp);
  pts->SetPoint(2, temp);
  findMidpoint(m_origin, m_pt2, temp);
  pts->SetPoint(3, temp);
  pts->SetPoint(4, m_midpoint);
  m_polyData->SetPoints(pts);
  pts->Delete();
}

void rtCrosshair2D::findMidpoint(double in1[3], double in2[3], double out[3]) {
  out[0] = (in1[0] + in2[0]) / 2.0f;
  out[1] = (in1[1] + in2[1]) / 2.0f;
  out[2] = (in1[2] + in2[2]) / 2.0f;
}
