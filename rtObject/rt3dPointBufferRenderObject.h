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
 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

 private:
  QList<SinglePointPipeline*> m_pipeList;
};

#endif
