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

#include "rtSingle3DPointPipeline.h"

rtSingle3DPointPipeline::rtSingle3DPointPipeline() {
  m_sphere = vtkSphereSource::New();
  m_mapper = vtkPolyDataMapper::New();
  m_actor = vtkActor::New();

  m_labelActor = vtkFollower::New();
  m_labelMapper = vtkPolyDataMapper::New();
  m_labelText = vtkVectorText::New();

  m_mapper->SetInput(m_sphere->GetOutput());
  m_actor->SetMapper(m_mapper);
  m_labelMapper->SetInputConnection(m_labelText->GetOutputPort());
  m_labelActor->SetMapper(m_labelMapper);
}

rtSingle3DPointPipeline::~rtSingle3DPointPipeline() {
  m_sphere->Delete();
  m_mapper->Delete();
  m_actor->Delete();
  m_labelMapper->Delete();
  m_labelText->Delete();
  m_labelActor->Delete();
}

void rtSingle3DPointPipeline::setPosition(double x, double y, double z) {
  m_sphere->SetCenter(x,y,z);
}

void rtSingle3DPointPipeline::setPosition(double coords[3]) {
  m_sphere->SetCenter(coords);
}

void rtSingle3DPointPipeline::setLabelPosition(double x, double y, double z) {
    m_labelActor->SetPosition(x,y,z);
}

void rtSingle3DPointPipeline::setLabelPosition(double coords[3]) {
    m_labelActor->SetPosition(coords);
}

void rtSingle3DPointPipeline::setRadius(double r) {
  if (r<=0.0) return;
  m_sphere->SetRadius(r);
}

void rtSingle3DPointPipeline::setLabelScale(double s)
{
    m_labelActor->SetScale(s);
}

void rtSingle3DPointPipeline::setLabelCamera(vtkCamera *cam)
{
    m_labelActor->SetCamera(cam);
}

void rtSingle3DPointPipeline::setLabelText(char *text)
{
    m_labelText->SetText(text);
}

void rtSingle3DPointPipeline::setResolution(int resolution) {
  if (resolution <= 0) return;

  m_sphere->SetThetaResolution(resolution);
  m_sphere->SetPhiResolution(resolution);
}

void rtSingle3DPointPipeline::setProperty(vtkProperty* prop) {
  if (!prop) return;
  m_actor->SetProperty(prop);
}
