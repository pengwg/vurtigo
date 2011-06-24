/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "Plane2DControlWidget.h"
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
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);

  //! Get the bounds in 3D for this object.
  virtual bool getObjectLocation(double loc[6]);

  //! Deselect this object
  virtual void deselect();

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event,int window);
  virtual void mouseReleaseEvent(QMouseEvent* event,int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event,int window);
  virtual void wheelEvent(QWheelEvent* event,int window);

 protected:
  //! Create the correct data object.
  void setupDataObject();

  //! Create the pipeline.
  void setupPipeline();

  //! Update the rendering with information from the data object
  void update();

  rtTexture2DPlane m_texturePlane;
  rtBox2DOutline m_boxOutline;
  Plane2DControlWidget m_control;

  vtkImageCast* m_imgCast;
  vtkImageMapper* m_imgMap;
  vtkActor2D* m_actor2D;

  // Blend streams of data.
  QList<vtkImageReslice*> m_reslice;
  QList<vtkImageMapToColors*> m_mapToColors;
  QList<vtkWindowLevelLookupTable*> m_colorFunction;
  vtkImageBlend* m_imageBlend;
 private:
  //! The position (in 2D) of a mouse click
  QPoint m_mousePos;

};

#endif
