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

#include "rtRenderObject.h"
#include "ObjectControlWidget.h"
#include "rtBox2DOutline.h"
#include "rtTexture2DPlane.h"

#include <vtkImageCast.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageMapToColors.h>
#include <vtkColorTransferFunction.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageBlend.h>
#include <vtkImageReslice.h>

//! A Single 2D Slice
/*!
  Render a single 2D slice in a specified orientation.
  */
class rt2DSliceRenderObject : public rtRenderObject {
 public:
  //! Constructor
  rt2DSliceRenderObject();
  //! Destructor
  ~rt2DSliceRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);

  //! Get the bounds in 3D for this object.
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
  //! Create the correct data object.
  void setupDataObject();

  //! Create the pipeline.
  void setupPipeline();

  //! Update the rendering with information from the data object
  void update();

  rtTexture2DPlane m_texturePlane;
  rtBox2DOutline m_boxOutline;
  ObjectControlWidget m_control;

  vtkImageCast* m_imgCast;
  vtkImageMapper* m_imgMap;
  vtkActor2D* m_actor2D;

  // Blend streams of data.
  QList<vtkImageReslice*> m_reslice;
  QList<vtkImageMapToColors*> m_mapToColors;
  QList<vtkWindowLevelLookupTable*> m_colorFunction;
  vtkImageBlend* m_imageBlend;
 private:

};

#endif
