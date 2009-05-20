#ifndef RT_4D_VOLUME_RENDER_OBJECT_H
#define RT_4D_VOLUME_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! Renderer for 3D Volume plus time.
/*!
  @todo Implement this class.
 */
class rt4DVolumeRenderObject : public rtRenderObject {

 public:

  rt4DVolumeRenderObject();
  ~rt4DVolumeRenderObject();

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
