#ifndef RT_PIECE_FUNC_RENDER_OBJECT_H
#define RT_PIECE_FUNC_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The renderer for the piecewise function.
/*!
  @todo Implement this class
  */
class rtPieceFuncRenderObject : public rtRenderObject {

 public:

  rtPieceFuncRenderObject();
  ~rtPieceFuncRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
