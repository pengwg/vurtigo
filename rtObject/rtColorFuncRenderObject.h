#ifndef RT_COLOR_FUNC_RENDER_OBJECT_H
#define RT_COLOR_FUNC_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! Class that implements the rendering of the color function
/*!
  @todo Implement this class
  */
class rtColorFuncRenderObject : public rtRenderObject {

 public:

  rtColorFuncRenderObject();
  ~rtColorFuncRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
