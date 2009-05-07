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

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
