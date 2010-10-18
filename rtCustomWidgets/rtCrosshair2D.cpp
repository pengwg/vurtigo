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
