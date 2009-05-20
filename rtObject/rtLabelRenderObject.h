#ifndef RT_LABEL_RENDER_OBJECT_H
#define RT_LABEL_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "vtkTextActor.h"

class rtLabelRenderObject : public rtRenderObject {
 public:
  rtLabelRenderObject();
  ~rtLabelRenderObject();

  void update();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
 protected:
  void setupDataObject();
  void setupPipeline();
 private:
  vtkTextActor *m_textActor2D;
};

#endif
