#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

//! A Single 2D Slice
/*!
  @todo Complete this class
  */
class rt2DSliceRenderObject : public rtRenderObject {

 public:

  rt2DSliceRenderObject();
  ~rt2DSliceRenderObject();

  void update();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
 protected:
  void setupDataObject();
  void setupPipeline();

  // Objects for the texture pipeline.
  vtkSmartPointer<vtkPlaneSource> m_texturePlane;
  vtkSmartPointer<vtkPolyDataMapper> m_planeMapper;
  vtkSmartPointer<vtkActor> m_textureActor;
  vtkSmartPointer<vtkTexture> m_texture;
  vtkSmartPointer<vtkImageMapToColors> m_imgMapToColors;
  vtkSmartPointer<vtkLookupTable> m_lookupTable;

  // Objects for the frame outline pipeline.
  vtkPolyData* m_outlinePolyData;
  vtkPolyDataMapper* m_outlineMapper;
  vtkActor* m_outlineActor;
  vtkProperty* m_outlineProperty;

 private:

};

#endif
