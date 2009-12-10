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
#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include "rtRenderObject.h"
#include "ObjectControlWidget.h"
#include "rtBox2DOutline.h"

//! A Single 2D Slice
/*!
  Render a single 2D slice in a specified orientation.
  */
class rt2DSliceRenderObject : public rtRenderObject {

 public:

  rt2DSliceRenderObject();
  ~rt2DSliceRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
  virtual bool getObjectLocation(double loc[6]);
public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  // Objects for the texture pipeline.
  vtkSmartPointer<vtkPlaneSource> m_texturePlane;
  vtkSmartPointer<vtkPolyDataMapper> m_planeMapper;
  vtkSmartPointer<vtkActor> m_textureActor;
  vtkSmartPointer<vtkTexture> m_texture;
  vtkSmartPointer<vtkImageMapToColors> m_imgMapToColors;
  vtkSmartPointer<vtkWindowLevelLookupTable> m_lookupTable;

  rtBox2DOutline m_boxOutline;
  ObjectControlWidget m_control;
 private:

};

#endif
