#ifndef RT_3D_VOLUME_RENDER_OBJECT_H
#define RT_3D_VOLUME_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkTransformFilter.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

//! The 3D Volume render object
/*!
  Rendering pipeline for the 3D object.
  */
class rt3DVolumeRenderObject : public rtRenderObject {

 public:

  rt3DVolumeRenderObject();
  ~rt3DVolumeRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

  vtkTransformFilter* m_tranformFilter;
  vtkVolumeRayCastMapper* m_rayMapper;
  vtkVolume* m_volumeActor;

  vtkOutlineFilter *m_outline;
  vtkPolyDataMapper *m_outlineMapper;
  vtkActor* m_outlineActor;

 private:

};

#endif
