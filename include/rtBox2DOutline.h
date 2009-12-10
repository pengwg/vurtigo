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
    \sa setBounds()
    \sa update()
    */
  void setTransform( vtkTransform* t );

  //! Set new bounds for the outline
  /*!
    Modify the bounds. Will not take effect until update() is called.
    \param bounds The bounds for the plane defined in order as: {xmin, xmax, ymin, ymax, zmin, zmax}.
    \sa setTransform
    \sa update()
    */
  void setBounds( double bounds[6] );


  //! Set the four points to the plane directly.
  /*!
    This is an alternate way to specify the position of the plane. Note that this function should not be used with setTransform and setBounds.
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

 protected:
  // Objects for the frame outline pipeline.
  vtkPolyData* m_outlinePolyData;
  vtkPolyDataMapper* m_outlineMapper;
  vtkActor* m_outlineActor;
  vtkProperty* m_outlineProperty;

  double m_bounds[6];
  vtkTransform* m_transform;
};

#endif // RTBOX2DOUTLINE_H
