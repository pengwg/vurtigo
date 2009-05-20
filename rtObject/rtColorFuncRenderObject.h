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
