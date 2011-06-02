#ifndef RT_POLY_RENDER_OBJECT_H
#define RT_POLY_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

//! The render object for polygonal data.
/*!
  Renders custom poly data.
  */
class rtPolyRenderObject : public rtRenderObject {

 public:

  rtPolyRenderObject();
  ~rtPolyRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  vtkSmartPointer<vtkPolyDataMapper> m_mapper;
  vtkSmartPointer<vtkActor> m_actor;

 private:

};

#endif
