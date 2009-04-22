#ifndef RT_NONE_RENDER_OBJECT_H
#define RT_NONE_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! A class that represents the minimum required for a new datatype.
class rtNoneRenderObject : public rtRenderObject {

 public:

  rtNoneRenderObject();
  ~rtNoneRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
