#ifndef RTTEXTURE2DPLANE_H
#define RTTEXTURE2DPLANE_H

#include <vtkPlaneSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkTransform.h>

#include "rtPolyData2D.h"

//! The 2D surface where the image data is displayed.
class rtTexture2DPlane : public rtPolyData2D
{
public:
  //! Constructor
  rtTexture2DPlane();

  //! Destructor
  ~rtTexture2DPlane();

  //! Adjust the scalar range for the window level function.
  void setScalarRange(double min, double max);
  void setWindow(double w);
  void setLevel(double l);

  //! Set the image data
  void setImageData(vtkImageData* img,bool isGrey);

  //! Update the widget.
  virtual void update();

protected:
  vtkPlaneSource* m_texturePlane;
  vtkTexture* m_texture;
  vtkImageMapToColors* m_imgMapToColors;
  vtkWindowLevelLookupTable* m_lookupTable;

};

#endif // RTTEXTURE2DPLANE_H
