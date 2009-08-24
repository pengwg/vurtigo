#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

//! A Single 2D Slice
/*!
  Render a single 2D slice in a specified orientation.
  */
class rt2DSliceRenderObject : public rtRenderObject {

 public:

  rt2DSliceRenderObject();
  ~rt2DSliceRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

  // Objects for the texture pipeline.
  vtkSmartPointer<vtkPlaneSource> m_texturePlane;
  vtkSmartPointer<vtkPolyDataMapper> m_planeMapper;
  vtkSmartPointer<vtkActor> m_textureActor;
  vtkSmartPointer<vtkTexture> m_texture;
  vtkSmartPointer<vtkImageMapToColors> m_imgMapToColors;
  vtkSmartPointer<vtkWindowLevelLookupTable> m_lookupTable;

  // Objects for the frame outline pipeline.
  vtkSmartPointer<vtkPolyData> m_outlinePolyData;
  vtkSmartPointer<vtkPolyDataMapper> m_outlineMapper;
  vtkSmartPointer<vtkActor> m_outlineActor;
  vtkSmartPointer<vtkProperty> m_outlineProperty;

 private:

};

#endif
