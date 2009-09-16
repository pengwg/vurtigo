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

#include "rtRenderObject.h"

//Qt
#include <QList>

//VTK 
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"



class rt3DPointBufferRenderObject : public rtRenderObject {

 public:

  class SinglePointPipeline {
  public:
    SinglePointPipeline() {
      sphere = vtkSphereSource::New();
      mapper = vtkPolyDataMapper::New();
      actor = vtkActor::New();

      mapper->SetInput(sphere->GetOutput());
      actor->SetMapper(mapper);
    }

    ~SinglePointPipeline() {
      sphere->Delete();
      mapper->Delete();
      actor->Delete();
    }

    // VTK pipeline objects.
    vtkSphereSource* sphere;
    vtkPolyDataMapper* mapper;
    vtkActor* actor;
  };

  rt3DPointBufferRenderObject();
  ~rt3DPointBufferRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

 private:
  QList<SinglePointPipeline*> m_pipeList;
};

#endif
