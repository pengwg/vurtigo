#ifndef RT_2D_POINT_RENDER_OBJECT_H
#define RT_2D_POINT_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The render object for 2D points.
/*!
  @todo Implement this class.
  */
class rt2DPointRenderObject : public rtRenderObject {

 public:

  rt2DPointRenderObject();
  ~rt2DPointRenderObject();

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
