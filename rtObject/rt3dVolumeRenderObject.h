#ifndef RT_3D_VOLUME_RENDER_OBJECT_H
#define RT_3D_VOLUME_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "rtImagePlaneWidget.h"

#include <vtkImageCast.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageCast.h>
#include <vtkImageActor.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>
#include <vtkImageReslice.h>


//! The 3D Volume render object
/*!
  Rendering pipeline for the 3D object.
  */
class rt3DVolumeRenderObject : public rtRenderObject {

 public:

  rt3DVolumeRenderObject();
  ~rt3DVolumeRenderObject();

  void update();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
 protected:
  void setupDataObject();
  void setupPipeline();
  void update3PlaneStatus();

  vtkImageReslice* m_transFilter;

  vtkVolumeRayCastMapper* m_rayMapper;
  vtkVolume* m_volumeActor;

  vtkOutlineFilter *m_outline;
  vtkPolyDataMapper *m_outlineMapper;
  vtkActor* m_outlineActor;

  // The 2D planes
  rtImagePlaneWidget *m_planes[3];
  vtkImageCast* m_imgCast[3];
  vtkImageMapper* m_imgMap[3];
  vtkActor2D* m_actor2D[3];
 private:

};

#endif
