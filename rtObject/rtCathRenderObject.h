#ifndef RT_CATH_RENDER_OBJECT_H
#define RT_CATH_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! The catheter render object
/*!
  @todo Implement this Catheter Render Object.
  */
class rtCathRenderObject : public rtRenderObject {

 public:

  rtCathRenderObject();
  ~rtCathRenderObject();

  void update();

 protected:
  void setupDataObject();
  void setupPipeline();

 private:

};

#endif
