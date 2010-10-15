/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkMath.h>

#include "rtBox2DOutline.h"


rtBox2DOutline::rtBox2DOutline() {

  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, 0.0, 0.0, 0.0);
  pts->SetPoint(1, 1.0, 0.0, 0.0);
  pts->SetPoint(2, 1.0, 1.0, 0.0);
  pts->SetPoint(3, 0.0, 1.0, 0.0);

  m_polyData->SetPoints(pts);
  pts->Delete();

  vtkCellArray* lines = vtkCellArray::New();
  lines->InsertNextCell(5);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);
  lines->InsertCellPoint(3);
  lines->InsertCellPoint(0);
  m_polyData->SetLines(lines);
  lines->Delete();
}


rtBox2DOutline::~rtBox2DOutline() {
}


void rtBox2DOutline::update() {
  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, m_origin);
  pts->SetPoint(1, m_pt1);
  pts->SetPoint(2, m_opposite);
  pts->SetPoint(3, m_pt2);
  m_polyData->SetPoints(pts);
  pts->Delete();
}

void rtBox2DOutline::setOutlineColor(double r, double g, double b) {
  setColor(r,g,b);
}


