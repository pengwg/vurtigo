#ifndef RT_3D_VOLUME_RENDER_OBJECT_H
#define RT_3D_VOLUME_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The 3D Volume render object
/*!
  @todo Implement this 3D Volume Render Object.
  */
class rt3DVolumeRenderObject : public rtRenderObject {

 public:

  rt3DVolumeRenderObject();
  ~rt3DVolumeRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
