#ifndef RT_CATH_RENDER_OBJECT_H
#define RT_CATH_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkTubeFilter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkKochanekSpline.h"

#include "vtkAppendPolyData.h"
#include "vtkSphereSource.h"

#include "vtkConeSource.h"

#include <QList>

//! The catheter render object
/*!
  Renders a catheter as a spline with points where coils are located.
  */
class rtCathRenderObject : public rtRenderObject {

 public:

  rtCathRenderObject();
  ~rtCathRenderObject();

  void update();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
 protected:
  void setupDataObject();
  void setupPipeline();

  // Spline Pipeline
  int m_numSplinePoints;
  vtkActor* m_splineActor;
  vtkPolyDataMapper* m_splineMapper;
  vtkTubeFilter* m_splineFilter;
  vtkPolyData* m_splineLineData;
  vtkCellArray* m_splineCellArray;
  vtkPoints* m_splinePoints;
  vtkKochanekSpline* m_spline[3];

  // Spheres Pipeline
  QList<vtkSphereSource*> m_sphereList;
  vtkAppendPolyData* m_sphereAppend;
  vtkPolyDataMapper* m_sphereMapper;
  vtkActor* m_sphereActor;

  // Cath Tip Pipeline
  vtkConeSource* m_coneSource;
  vtkPolyDataMapper* m_coneMapper;
  vtkActor* m_coneActor;

 private:

};

#endif
