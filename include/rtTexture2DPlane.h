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
    \sa setSize()
    \sa update()
    */
  void setTransform( vtkTransform* t );

  //! Set new sizes for the texture
  /*!
    Modify the size. Will not take effect until update() is called.
    \param xsize The size in the x direction. The height.
    \param ysize The size in the y direction. he width.
    \sa setTransform
    \sa update()
    */
  void setSize( double xsize, double ysize );

    //! Set the four points to the plane directly.
  /*!
    This is an alternate way to specify the position of the plane. Note that this function should not be used with setTransform and setSize.
    The points are listed clockwise around the plane. The opposite point is not required. The update() call is NOT NEEDED as this function updates on the spot.
    \param orig The origin
    \param p1 Point clockwise of the origin
    \param p2 Point counter-clockwise of the origin
    */
  void setCorners(double orig[3], double p1[3], double p2[3]);

  //! Update the widget.
  void update();

protected:
  vtkPlaneSource* m_texturePlane;
  vtkPolyDataMapper* m_planeMapper;
  vtkActor* m_textureActor;
  vtkTexture* m_texture;
  vtkImageMapToColors* m_imgMapToColors;
  vtkWindowLevelLookupTable* m_lookupTable;

  double m_xsize;
  double m_ysize;
  vtkTransform* m_transform;

  //! Normalize a given vector.
  void normalizeVec(double vec[3]);
};

#endif // RTTEXTURE2DPLANE_H
