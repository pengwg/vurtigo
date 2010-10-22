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
#ifndef RT_CATH_RENDER_OBJECT_H
#define RT_CATH_RENDER_OBJECT_H

// VTK
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTubeFilter.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkKochanekSpline.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkSmartPointer.h>

#include <QList>

// Local
#include "rtRenderObject.h"
#include "rtArrowPipeline.h"

//! The catheter render object
/*!
  Renders a catheter as a spline with points where coils are located.
  */
class rtCathRenderObject : public rtRenderObject {

 public:

  rtCathRenderObject();
  ~rtCathRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
  virtual bool getObjectLocation(double loc[6]);

  void visibilityOn();
  void visibilityOff();

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  // Spline Pipeline
  rtArrowPipeline m_cathArrow;

  vtkSmartPointer<vtkKochanekSpline> m_spline[3];

  // Spheres Pipeline
  QList<vtkSphereSource*> m_sphereList;
  vtkSmartPointer<vtkAppendPolyData> m_sphereAppend;
  vtkSmartPointer<vtkPolyDataMapper> m_sphereMapper;
  vtkSmartPointer<vtkActor> m_sphereActor;

 private:

};

#endif
