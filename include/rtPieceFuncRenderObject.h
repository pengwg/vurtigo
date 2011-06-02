#ifndef RT_PIECE_FUNC_RENDER_OBJECT_H
#define RT_PIECE_FUNC_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The renderer for the piecewise function.
/*!
  The piecewise function is not rendered. It is used as a parameter to renderable objects.
  */
class rtPieceFuncRenderObject : public rtRenderObject {

 public:

  rtPieceFuncRenderObject();
  ~rtPieceFuncRenderObject();


  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

 private:

};

#endif
