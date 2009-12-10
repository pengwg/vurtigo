/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RTTEXTURE2DPLANE_H
#define RTTEXTURE2DPLANE_H

#include <vtkPlaneSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkTransform.h>

//! The 2D surface where the image data is displayed.
class rtTexture2DPlane
{
public:
  //! Constructor
  rtTexture2DPlane();

  //! Destructor
  ~rtTexture2DPlane();

  //! Get the actor for this plane.
  vtkActor* getActor() { return m_textureActor; }

  //! Adjust the scalar range for the window level function.
  void setScalarRange(double min, double max);
  void setWindow(double w);
  void setLevel(double l);

  //! Set the image data
  void setImageData(vtkImageData* img);

  //! Set a new trasform for the texture.
  /*!
    The transform will be modified within the class but the GUI will not be updated until update() is called.
    Don't forget to call update() for the changes to take effect.
    \sa setBounds()
    \sa update()
    */
  void setTransform( vtkTransform* t );

  //! Set new bounds for the texture
  /*!
    Modify the bounds. Will not take effect until update() is called.
    \param bounds The bounds for the plane defined in order as: {xmin, xmax, ymin, ymax, zmin, zmax}.
    \sa setTransform
    \sa update()
    */
  void setBounds( double bounds[6] );

  //! Update the widget.
  void update();

protected:
  vtkPlaneSource* m_texturePlane;
  vtkPolyDataMapper* m_planeMapper;
  vtkActor* m_textureActor;
  vtkTexture* m_texture;
  vtkImageMapToColors* m_imgMapToColors;
  vtkWindowLevelLookupTable* m_lookupTable;

  double m_bounds[6];
  vtkTransform* m_transform;
};

#endif // RTTEXTURE2DPLANE_H
