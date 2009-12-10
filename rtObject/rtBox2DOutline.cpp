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

#include "rtBox2DOutline.h"


rtBox2DOutline::rtBox2DOutline() {
  m_outlinePolyData = vtkPolyData::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();
  m_outlineProperty = vtkProperty::New();
  m_transform = vtkTransform::New();

  // Default bounds.
  m_bounds[0] = 0.0;
  m_bounds[1] = 1.0;
  m_bounds[2] = 0.0;
  m_bounds[3] = 1.0;
  m_bounds[4] = 0.0;
  m_bounds[5] = 1.0;

  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, 0.0, 0.0, 0.0);
  pts->SetPoint(1, 1.0, 0.0, 0.0);
  pts->SetPoint(2, 1.0, 1.0, 0.0);
  pts->SetPoint(3, 0.0, 1.0, 0.0);

  m_outlinePolyData->SetPoints(pts);
  pts->Delete();

  vtkCellArray* lines = vtkCellArray::New();
  lines->InsertNextCell(5);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);
  lines->InsertCellPoint(3);
  lines->InsertCellPoint(0);
  m_outlinePolyData->SetLines(lines);
  lines->Delete();

  m_outlineMapper->SetInput(m_outlinePolyData);
  m_outlineActor->SetMapper(m_outlineMapper);
  m_outlineActor->SetProperty(m_outlineProperty);


}


rtBox2DOutline::~rtBox2DOutline() {
  if (m_outlinePolyData) m_outlinePolyData->Delete();
  if (m_outlineMapper) m_outlineMapper->Delete();
  if (m_outlineActor) m_outlineActor->Delete();
  if (m_outlineProperty) m_outlineProperty->Delete();
  if (m_transform) m_transform->Delete();
}

void rtBox2DOutline::setTransform( vtkTransform* t ) {
  m_transform->DeepCopy(t);
}

void rtBox2DOutline::setBounds( double bounds[6] ) {
  for (int ix1=0; ix1<6; ix1++) m_bounds[ix1] = bounds[ix1];
}

void rtBox2DOutline::setCorners(double orig[3], double pt1[3], double opp[3], double pt2[3]) {
  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, orig);
  pts->SetPoint(1, pt1);
  pts->SetPoint(2, opp);
  pts->SetPoint(3, pt2);
  m_outlinePolyData->SetPoints(pts);
  pts->Delete();
}

void rtBox2DOutline::update() {
  double orig[3], pt1[3], pt2[3], opp[3];
  double xsize, ysize, zsize;

  xsize = m_bounds[1]-m_bounds[0];
  ysize = m_bounds[3]-m_bounds[2];

  // The Z size should be one...
  zsize = m_bounds[5]-m_bounds[4];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*ysize; pt2[2]=0.0;
  opp[0]=1.0*xsize; opp[1]=1.0*ysize; opp[2]=0.0;

  m_transform->TransformPoint(orig, orig);
  m_transform->TransformPoint(pt1, pt1);
  m_transform->TransformPoint(pt2, pt2);
  m_transform->TransformPoint(opp, opp);

  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, orig);
  pts->SetPoint(1, pt1);
  pts->SetPoint(2, opp);
  pts->SetPoint(3, pt2);
  m_outlinePolyData->SetPoints(pts);
  pts->Delete();
}

void rtBox2DOutline::setOutlineColor(double r, double g, double b) {
  m_outlineProperty->SetColor(r,g,b);
}
