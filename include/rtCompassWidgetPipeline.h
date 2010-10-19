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

#ifndef RTCOMPASSWIDGETPIPELINE_H
#define RTCOMPASSWIDGETPIPELINE_H

#include <vtkTransform.h>
#include <vtkParametricTorus.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>

//! Complete rendering pipeline for a single point in 3D space.
class rtCompassWidgetPipeline
{
public:
  rtCompassWidgetPipeline();
  ~rtCompassWidgetPipeline();

  inline vtkActor* getActor(int index) { return m_diskActor[index]; }

  //! Get the position transform as a particular index.
  vtkTransform* getPositionTransform(int index);

  //! Apply the position transforms to all the actors.
  void applyPositionTransform();

  //! Get the direction of positive rotation.
  /*!
    This function will accept the position of a click in 3D space and will determine the direction in which the wheel must spin for
    the rotation to be considered positive. The idea is that if the mouse moves in the same direction as the positive rotation vector then the wheel will
    spin in the direction of the angle.
    \param index The index of the plane to use for the calculation
    \param clickPosition The position in (x,y,z) of the given click
    \param positiveRotation The vector that represents the direction of positive rotation.
    */
  void getPositiveRotationDirection(int index, double clickPosition[3], double positiveRotation[3]);

  //! Set the radius of the rings
  void setRadius(int index, double r);

  //! Set the cross-sectional readius for the spheres
  void setCrossSectionRadius(int index, double r);

  //! Graphics resolution
  void setResolution(int resolution);

  void setProperty(int index, vtkProperty* prop);

  inline vtkProperty* getPropertyHandle(int index) { return m_diskActor[index]->GetProperty(); }

protected:
  // VTK pipeline objects.
  vtkTransform *m_position[3];
  vtkParametricTorus *m_torus[3];
  vtkParametricFunctionSource *m_torusSrc[3];
  vtkPolyDataMapper *m_diskMapper[3];
  vtkActor *m_diskActor[3];

};

#endif
