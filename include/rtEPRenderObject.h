#ifndef RT_EP_RENDER_OBJECT_H
#define RT_EP_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>


//! The render object for EP data.
/*!
  Renders custom EP data.
  */
class rtEPRenderObject : public rtRenderObject {

 public:
  //! Constructor
  rtEPRenderObject();

  //! Destructor
  ~rtEPRenderObject();

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
  vtkPolyDataMapper* m_pointMapper;
  vtkPolyDataMapper* m_meshMapper;
  vtkPolyDataMapper* m_infoMapper;
  vtkActor* m_pointActor;
  vtkActor* m_meshActor;
  vtkActor* m_infoActor;
};

#endif
