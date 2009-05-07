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

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
