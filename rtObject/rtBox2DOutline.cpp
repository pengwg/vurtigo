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
  m_outlinePolyData = vtkPolyData::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();
  m_outlineProperty = vtkProperty::New();
  m_transform = vtkTransform::New();

  // Default sizes.
  m_xsize = 1.0f;
  m_ysize = 1.0f;

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
  m_transform->SetMatrix(t->GetMatrix());
}

void rtBox2DOutline::setSize( double xsize, double ysize ) {
  m_xsize = xsize;
  m_ysize = ysize;
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

  // Setup the parameters.
  for (int ix1=0; ix1<3; ix1++) {
    m_origin[ix1] = orig[ix1];
    m_pt1[ix1] = pt1[ix1];
    m_pt2[ix1] = pt2[ix1];
    m_opposite[ix1] = opp[ix1];
    m_midpoint[ix1] = (m_origin[ix1]+m_opposite[ix1])/2.0f;
  }

  m_xsize = sqrt(vtkMath::Distance2BetweenPoints(m_pt1, m_origin));
  m_ysize = sqrt(vtkMath::Distance2BetweenPoints(m_pt2, m_origin));
  // Update the sizes

  // Update the transform too.
  m_transform->Identity();

  double xd[3];
  double yd[3];
  double zd[3];

  for (int ix1=0; ix1<3; ix1++) {
    xd[ix1]=pt1[ix1]-orig[ix1];
    yd[ix1]=pt2[ix1]-orig[ix1];
  }

  vtkMath::Normalize(xd);
  vtkMath::Normalize(yd);
  vtkMath::Cross(xd, yd, zd);

  vtkMatrix4x4 *temp = vtkMatrix4x4::New();
  for (int ix1=0; ix1<3; ix1++) {
    temp->SetElement(ix1, 0, xd[ix1]);
    temp->SetElement(ix1, 1, yd[ix1]);
    temp->SetElement(ix1, 2, zd[ix1]);
    temp->SetElement(ix1, 3, orig[ix1]);
  }
  m_transform->SetMatrix(temp);
  temp->Delete();
}

void rtBox2DOutline::update() {
  double orig[3], pt1[3], pt2[3], opp[3];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*m_xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*m_ysize; pt2[2]=0.0;
  opp[0]=1.0*m_xsize; opp[1]=1.0*m_ysize; opp[2]=0.0;

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

  // Setup the parameters.
  for (int ix1=0; ix1<3; ix1++) {
    m_origin[ix1] = orig[ix1];
    m_pt1[ix1] = pt1[ix1];
    m_pt2[ix1] = pt2[ix1];
    m_opposite[ix1] = opp[ix1];
    m_midpoint[ix1] = (m_origin[ix1]+m_opposite[ix1])/2.0f;
  }

  m_xsize = sqrt(vtkMath::Distance2BetweenPoints(m_pt1, m_origin));
  m_ysize = sqrt(vtkMath::Distance2BetweenPoints(m_pt2, m_origin));
}

void rtBox2DOutline::setOutlineColor(double r, double g, double b) {
  m_outlineProperty->SetColor(r,g,b);
}

void rtBox2DOutline::getOrigin( double orig[3] ) {
  orig[0] = m_origin[0];
  orig[1] = m_origin[1];
  orig[2] = m_origin[2];
}

void rtBox2DOutline::getMidpoint( double mid[3] ) {
  mid[0] = m_midpoint[0];
  mid[1] = m_midpoint[1];
  mid[2] = m_midpoint[2];
}

void rtBox2DOutline::getOpposite( double opp[3] ) {
  opp[0] = m_opposite[0];
  opp[1] = m_opposite[1];
  opp[2] = m_opposite[2];
}

void rtBox2DOutline::getVector1( double v1[3] ) {
  v1[0] = m_origin[0] - m_pt1[0];
  v1[1] = m_origin[1] - m_pt1[1];
  v1[2] = m_origin[2] - m_pt1[2];
}

void rtBox2DOutline::getVector2( double v2[3] ) {
  v2[0] = m_origin[0] - m_pt2[0];
  v2[1] = m_origin[1] - m_pt2[1];
  v2[2] = m_origin[2] - m_pt2[2];
}

void rtBox2DOutline::getNormal( double n[3] ) {
  double v1[3];
  double v2[3];
  getVector1(v1);
  getVector2(v2);
  vtkMath::Cross(v1, v2, n);
}
