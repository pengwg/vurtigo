#ifndef RT_LABEL_RENDER_OBJECT_H
#define RT_LABEL_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "vtkTextActor.h"

class rtLabelRenderObject : public rtRenderObject {
 public:
  rtLabelRenderObject();
  ~rtLabelRenderObject();



 protected:
  void setupDataObject();
  void setupRenderOptions();
  void setupPipeline();
 private:
  vtkTextActor *m_textActor2D;
};

#endif
