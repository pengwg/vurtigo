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


