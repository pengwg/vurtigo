#ifndef RT_MATRIX_RENDER_OBJECT_H
#define RT_MATRIX_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! Matrix Object
/*!
  @todo Implement this object
  */
class rtMatrixRenderObject : public rtRenderObject {

 public:

  rtMatrixRenderObject();
  ~rtMatrixRenderObject();

  void update();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
