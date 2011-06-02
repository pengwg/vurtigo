#ifndef RT_LABEL_RENDER_OBJECT_H
#define RT_LABEL_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "vtkTextActor.h"

class rtLabelRenderObject : public rtRenderObject {
 public:
  rtLabelRenderObject();
  ~rtLabelRenderObject();

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
  vtkTextActor *m_textActor2D;
};

#endif
