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
#ifndef RT3D_POINT_BUFFER_RENDER_OBJECT_H
#define RT3D_POINT_BUFFER_RENDER_OBJECT_H
//Qt
#include <QList>
#include <QHash>

#include "rtRenderObject.h"
#include "rtSingle3DPointPipeline.h"

//! A group of points in 3D that are rendered as a group of spheres.
class rt3DPointBufferRenderObject : public rtRenderObject {

 public:
  rt3DPointBufferRenderObject();
  ~rt3DPointBufferRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);

  //! The position of the center of the cluster of points
  virtual bool getObjectLocation(double loc[6]);

  //! Set the render quality as a number in the range [0, 1].
  /*!
    Values above 1 of below 0 will be truncated to 1 and 0 respectively. The best quality is 1.
    */
  virtual void setRenderQuality(double quality);

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

 private:
  //! List of point pipelines.
  QList<rtSingle3DPointPipeline*> m_pipeList;
};

#endif
