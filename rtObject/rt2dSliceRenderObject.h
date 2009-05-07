#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! A Single 2D Slice
/*!
  @todo Implement this class
  */
class rt2DSliceRenderObject : public rtRenderObject {

 public:

  rt2DSliceRenderObject();
  ~rt2DSliceRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
