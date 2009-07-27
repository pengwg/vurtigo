#ifndef RT_IMAGE_BUFFER_RENDER_OBJECT_H
#define RT_IMAGE_BUFFER_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The render object for a Buffer of 2D Images
/*!
  @todo Implement this class.
  */
class rtImageBufferRenderObject : public rtRenderObject {

 public:

  rtImageBufferRenderObject();
  ~rtImageBufferRenderObject();

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

};

#endif
