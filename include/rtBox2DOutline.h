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
#ifndef RTBOX2DOUTLINE_H
#define RTBOX2DOUTLINE_H

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTransform.h>

//! Creates the pipeline and the final actor for a set of four lines in the shape of a box.
class rtBox2DOutline
{
 public:
  //! Constructor
  rtBox2DOutline();

  //! Destructor
  ~rtBox2DOutline();

  //! Get a pointer to the actor object so it can be displayed
  vtkActor* getActor() { return m_outlineActor; }

  //! Set a new trasform for the outline.
  /*!
    The transform will be modified within the class but the GUI will not be updated until update() is called.
    Don't forget to call update() for the changes to take effect.
    \sa setSize()
    \sa update()
    */
  void setTransform( vtkTransform* t );

  //! Get the transform for the outline.
  vtkTransform* getTransform() { return m_transform; }

  //! Set the x and y sizes for the outline.
  /*!
    Modify the sizes. Will not take effect until update() is called.
    \param xsize The the size in the x direction. The plane height.
    \param ysize The size in the y direction. The plane width.
    \sa setTransform
    \sa update()
    */
  void setSize( double xsize, double ysize );


  //! Set the four points to the plane directly.
  /*!
    This is an alternate way to specify the position of the plane. Note that this function should not be used with setTransform and setSize.
    The points are listed clockwise around the plane. The update() call is NOT NEEDED as this function updates on the spot.
    \param orig The origin
    \param p1 Point clockwise of the origin
    \param opp Point opposite of the origin
    \param p2 Point counter-clockwise of the origin
    */
  void setCorners(double orig[3], double p1[3], double opp[3], double p2[3]);

  //! Update the widget.
  void update();

  //! Set the color of the outline
  /*!
    Set the color as rgb. The range for each value is in [0, 1].
    \param r The red component [0,1]
    \param g The green component [0,1]
    \param b The blue component [0,1]
    */
  void setOutlineColor(double r, double g, double b);

  //! The the origin of the outline
  void getOrigin( double orig[3] );

  //! Get the midpoint of the plane. This dictates the translation of the plane.
  void getMidpoint( double mid[3] );

  //! Get the point opposite from the origin.
  void getOpposite( double opp[3] );

  void getVector1( double v1[3] );
  void getVector2( double v2[3] );

  //! Get the normal to the plane.
  void getNormal( double n[3] );

  double getXSize() { return m_xsize; }
  double getYSize() { return m_ysize; }

 protected:
  // Objects for the frame outline pipeline.
  vtkPolyData* m_outlinePolyData;
  vtkPolyDataMapper* m_outlineMapper;
  vtkActor* m_outlineActor;
  vtkProperty* m_outlineProperty;

  double m_xsize;
  double m_ysize;
  vtkTransform* m_transform;
  double m_origin[3];
  double m_midpoint[3];
  double m_opposite[3];
  double m_pt1[3];
  double m_pt2[3];
};

#endif // RTBOX2DOUTLINE_H
