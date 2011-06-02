#ifndef RT_MATRIX_RENDER_OBJECT_H
#define RT_MATRIX_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>

//! Matrix Object
/*!
  Renders a plane to represent the rotation of the matrix.
  */
class rtMatrixRenderObject : public rtRenderObject {

 public:

  rtMatrixRenderObject();
  ~rtMatrixRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  vtkSmartPointer<vtkPlaneSource> m_planeSource;
  vtkSmartPointer<vtkPolyDataMapper> m_mapper;
  vtkSmartPointer<vtkActor> m_actor;
  vtkSmartPointer<vtkProperty> m_property;
 private:

};

#endif
