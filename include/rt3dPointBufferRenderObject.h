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
#ifndef RT3D_POINT_BUFFER_RENDER_OBJECT_H
#define RT3D_POINT_BUFFER_RENDER_OBJECT_H
//Qt
#include <QList>
#include <QHash>

// VTK
#include <vtkTransform.h>

// Local
#include "rtRenderObject.h"
#include "rtSingle3DPointPipeline.h"
#include "Points3DControlWidget.h"

//! A group of points in 3D that are rendered as a group of spheres.
class rt3DPointBufferRenderObject : public rtRenderObject {
  Q_OBJECT

 public:
  rt3DPointBufferRenderObject();
  ~rt3DPointBufferRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren, int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren, int window);

  //! The position of the center of the cluster of points
  virtual bool getObjectLocation(double loc[6]);

  //! Set the render quality as a number in the range [0, 1].
  /*!
    Values above 1 of below 0 will be truncated to 1 and 0 respectively. The best quality is 1.
    */
  virtual void setRenderQuality(double quality);

  //! deselect this object
  virtual void deselect();

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event, int window);
  virtual void mouseReleaseEvent(QMouseEvent* event, int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event, int window);
  virtual void wheelEvent(QWheelEvent* event, int window);

  void renderWindowChanged();

 protected:
  //! Create the correct data object.
  void setupDataObject();

  //! Create the part of the pipeline that is done first.
  void setupPipeline();

  void update();

  //! Perform a proper cleanup on the m_pipeList object.
  void cleanupPipeList();

  //! Change the size of the pipe list.
  /*!
    If the list is too long elements from the end will be removed. If it is too short elements will be added to the end.
    */
  void resizePipeList(int size);

  //! Set the new transform that arrived from the control widget.
  void setControlTransform(vtkTransform* t);

  Points3DControlWidget m_controlWidget;

  vtkTransform* m_currTransform;

 private:
  //! List of point pipelines.
  QList<rtSingle3DPointPipeline*> m_pipeList;
  vtkSphereSource *m_sphere;
  vtkPolyDataMapper *m_mapper;
};

#endif
