#ifndef RT_NONE_RENDER_OBJECT_H
#define RT_NONE_RENDER_OBJECT_H

#include "rtRenderObject.h"

//! A class that represents the minimum required for a new datatype.
class rtNoneRenderObject : public rtRenderObject {

 public:

  rtNoneRenderObject();
  ~rtNoneRenderObject();


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
