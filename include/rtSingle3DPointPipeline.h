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

#ifndef RTSINGLE3DPOINTPIPELINE_H
#define RTSINGLE3DPOINTPIPELINE_H

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

//! Complete rendering pipeline for a single point in 3D space.
class rtSingle3DPointPipeline
{
public:
  rtSingle3DPointPipeline();
  ~rtSingle3DPointPipeline();

  inline vtkActor* getActor() { return m_actor; }

  void setPosition(double x, double y, double z);

  //! Set the radius of the displayed point.
  void setRadius(double r);

  void setResolution(int resolution);

  void setProperty(vtkProperty* prop);

protected:
  // VTK pipeline objects.
  vtkSphereSource* m_sphere;
  vtkPolyDataMapper* m_mapper;
  vtkActor* m_actor;
};

#endif // RTSINGLE3DPOINTPIPELINE_H