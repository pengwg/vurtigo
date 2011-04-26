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
#ifndef RT_MATRIX_RENDER_OBJECT_H
#define RT_MATRIX_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>

//! Matrix Object
/*!
  Renders a plane to represent the rotation of the matrix.
  */
class rtMatrixRenderObject : public rtRenderObject {

 public:

  rtMatrixRenderObject();
  ~rtMatrixRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event, int window);
  virtual void mouseReleaseEvent(QMouseEvent* event, int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event, int window);
  virtual void keyPressEvent(QKeyEvent* event, int window);
  virtual void keyReleaseEvent(QKeyEvent* event, int window);
  virtual void wheelEvent(QWheelEvent* event, int window);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  vtkSmartPointer<vtkPlaneSource> m_planeSource;
  vtkSmartPointer<vtkPolyDataMapper> m_mapper;
  vtkSmartPointer<vtkActor> m_actor;
  vtkSmartPointer<vtkProperty> m_property;
 private:

};

#endif
