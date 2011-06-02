#include "rtCompassWidgetPipeline.h"

rtCompassWidgetPipeline::rtCompassWidgetPipeline() {
  // Create the pipeline for the 3 rings
  for (int ix1=0; ix1<3; ix1++) {
    m_position[ix1] = vtkTransform::New();
    m_torus[ix1] = vtkParametricTorus::New();
    m_torusSrc[ix1] = vtkParametricFunctionSource::New();
    m_diskMapper[ix1] = vtkPolyDataMapper::New();
    m_diskActor[ix1] = vtkActor::New();

    m_torusSrc[ix1]->SetParametricFunction(m_torus[ix1]);
    m_diskMapper[ix1]->SetInputConnection(m_torusSrc[ix1]->GetOutputPort());
    m_diskActor[ix1]->SetMapper(m_diskMapper[ix1]);
  }
}

rtCompassWidgetPipeline::~rtCompassWidgetPipeline() {
  for (int ix1=0; ix1<3; ix1++) {
    m_position[ix1]->Delete();
    m_torus[ix1]->Delete();
    m_torusSrc[ix1]->Delete();
    m_diskMapper[ix1]->Delete();
    m_diskActor[ix1]->Delete();
  }
}

vtkTransform* rtCompassWidgetPipeline::getPositionTransform(int index) {
  if (index < 0 || index > 2) return NULL;
  return m_position[index];
}

void rtCompassWidgetPipeline::applyPositionTransform() {
  for (int ix1=0; ix1<3; ix1++) {
    m_diskActor[ix1]->SetUserTransform(m_position[ix1]);
  }
}


void rtCompassWidgetPipeline::getPositiveRotationDirection(int index, double clickPosition[3], double positiveRotation[3]) {
  double pos2[3];
  double posDirec[3];

  m_position[index]->Inverse();
  m_position[index]->TransformPoint(clickPosition, pos2);
  m_position[index]->Inverse();

  if (pos2[0] > 0 && pos2[1] > 0) {
    posDirec[0] = pos2[0]+50.0;
    posDirec[1] = pos2[1]-50.0;
  } else if (pos2[0] > 0 && pos2[1] <= 0) {
    posDirec[0] = pos2[0]-50.0;
    posDirec[1] = pos2[1]-50.0;
  } else if (pos2[0] <= 0 && pos2[1] <= 0) {
    posDirec[0] = pos2[0]-50.0;
    posDirec[1] = pos2[1]+50.0;
  } else {
    posDirec[0] = pos2[0]+50.0;
    posDirec[1] = pos2[1]+50.0;
  }
  posDirec[2] = pos2[2]+0.0f;
  m_position[index]->TransformPoint(posDirec, positiveRotation);
}


void rtCompassWidgetPipeline::setRadius(int index, double r) {
  if (r<=0.0 || index < 0 || index > 2) return;
  m_torus[index]->SetRingRadius(r);
}

void rtCompassWidgetPipeline::setCrossSectionRadius(int index, double r) {
  if (r<=0.0 || index < 0 || index > 2) return;
  m_torus[index]->SetCrossSectionRadius(r);
}

void rtCompassWidgetPipeline::setResolution(int resolution) {
  if (resolution <= 0) return;

}

void rtCompassWidgetPipeline::setProperty(int index, vtkProperty* prop) {
  if (!prop) return;
  m_diskActor[index]->SetProperty(prop);
}
