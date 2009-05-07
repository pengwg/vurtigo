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

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
