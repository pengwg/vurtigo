#ifndef RT_CATH_RENDER_OBJECT_H
#define RT_CATH_RENDER_OBJECT_H

// VTK
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTubeFilter.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkKochanekSpline.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkSmartPointer.h>

#include <QList>

// Local
#include "rtRenderObject.h"
#include "rtArrowPipeline.h"

//! The catheter render object
/*!
  Renders a catheter as a spline with points where coils are located.
  */
class rtCathRenderObject : public rtRenderObject {

 public:

  rtCathRenderObject();
  ~rtCathRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

  void visibilityOn();
  void visibilityOff();

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  // Spline Pipeline
  rtArrowPipeline m_cathArrow;

  vtkSmartPointer<vtkKochanekSpline> m_spline[3];

  // Spheres Pipeline
  vtkSphereSource* m_sphere;
  vtkPolyDataMapper* m_sphereMapper;
  QList<vtkActor*> m_sphereActor;

 private:

};

#endif
