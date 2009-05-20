#ifndef RT_POLY_RENDER_OBJECT_H
#define RT_POLY_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The render object for polygonal data.
/*!
  @todo Implement this class.
  */
class rtPolyRenderObject : public rtRenderObject {

 public:

  rtPolyRenderObject();
  ~rtPolyRenderObject();

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
