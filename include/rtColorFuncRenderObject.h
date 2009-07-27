#ifndef RT_COLOR_FUNC_RENDER_OBJECT_H
#define RT_COLOR_FUNC_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! Class that implements the rendering of the color function
/*!
  Color Transfer Functions are not rendered. They are used as parameters for renderable objects.
  */
class rtColorFuncRenderObject : public rtRenderObject {

 public:

  rtColorFuncRenderObject();
  ~rtColorFuncRenderObject();

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
