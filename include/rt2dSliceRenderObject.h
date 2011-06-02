#ifndef RT_2D_SLICE_RENDER_OBJECT_H
#define RT_2D_SLICE_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "Plane2DControlWidget.h"
#include "rtBox2DOutline.h"
#include "rtTexture2DPlane.h"

#include <vtkImageCast.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageMapToColors.h>
#include <vtkColorTransferFunction.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageBlend.h>
#include <vtkImageReslice.h>

//! A Single 2D Slice
/*!
  Render a single 2D slice in a specified orientation.
  */
class rt2DSliceRenderObject : public rtRenderObject {
 public:
  //! Constructor
  rt2DSliceRenderObject();
  //! Destructor
  ~rt2DSliceRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);

  //! Get the bounds in 3D for this object.
  virtual bool getObjectLocation(double loc[6]);

  //! Deselect this object
  virtual void deselect();

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event,int window);
  virtual void mouseReleaseEvent(QMouseEvent* event,int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event,int window);
  virtual void wheelEvent(QWheelEvent* event,int window);

 protected:
  //! Create the correct data object.
  void setupDataObject();

  //! Create the pipeline.
  void setupPipeline();

  //! Update the rendering with information from the data object
  void update();

  rtTexture2DPlane m_texturePlane;
  rtBox2DOutline m_boxOutline;
  Plane2DControlWidget m_control;

  vtkImageCast* m_imgCast;
  vtkImageMapper* m_imgMap;
  vtkActor2D* m_actor2D;

  // Blend streams of data.
  QList<vtkImageReslice*> m_reslice;
  QList<vtkImageMapToColors*> m_mapToColors;
  QList<vtkWindowLevelLookupTable*> m_colorFunction;
  vtkImageBlend* m_imageBlend;
 private:
  //! The position (in 2D) of a mouse click
  QPoint m_mousePos;

};

#endif
