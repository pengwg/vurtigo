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

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren, int window);

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
