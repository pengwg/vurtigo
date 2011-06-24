/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef RTPOLYDATA2D_H
#define RTPOLYDATA2D_H

// VTK
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTransform.h>

//! Generic poly data 2D pipeline.
/*!
  This class will create the pipeline but will not add data to it.
  */
class rtPolyData2D
{
 public:
  rtPolyData2D();
  ~rtPolyData2D();

  //! Get a pointer to the actor object so it can be displayed
  inline vtkActor* const getActor() { return m_actor; }

  //! Get a pointer to the actor's property object
  inline vtkProperty* const getProperty() { return m_property; }

  //! Get the transform.
  inline vtkTransform* const getTransform() { return m_transform; }

  //! Set a new trasform.
  /*!
    The transform will be modified within the class and update() will be called at the end of the function.
    \sa setSize()
    \sa update()
    */
  void setTransform( vtkTransform* t );

  //! Set the four points to the plane directly.
  /*!
    This is an alternate way to specify the position of the plane. The points are listed clockwise around the plane.
    The update() function is called at the end.
    \param orig The origin
    \param p1 Point clockwise of the origin
    \param opp Point opposite of the origin
    \param p2 Point counter-clockwise of the origin
    */
  void setCorners(double orig[3], double p1[3], double opp[3], double p2[3]);

  //! Set the corners to the plane directly.
  /*!
    This is an alternate way to specify the position of the plane.
    The points are listed clockwise around the plane. The opposite point is not required as it is calculated from the other three.
    \param orig The origin
    \param p1 Point clockwise of the origin
    \param p2 Point counter-clockwise of the origin
    */
  void setCorners(double orig[3], double p1[3], double p2[3]);

  //! Set the x and y sizes for the outline.
  /*!
    Modify the sizes but leave the transform as it is. update() is called at the end.
    \param xsize The the size in the x direction. The plane height.
    \param ysize The size in the y direction. The plane width.
    \sa setTransform
    \sa update()
    */
  void setSize( double xsize, double ysize );

  //! Set the color of the actor
  /*!
    Set the color as rgb. The range for each value is in [0, 1].
    \param r The red component [0,1]
    \param g The green component [0,1]
    \param b The blue component [0,1]
    */
  void setColor(double r, double g, double b);

  inline void setVisible(bool v) {
    if (v) m_actor->VisibilityOn();
    else m_actor->VisibilityOff();
  }

  inline bool getVisible() {
    return (m_actor->GetVisibility() == 1);
  }

  //! Set an extra transform that only changes the final position of the actor.
  inline void setUserTransform(vtkTransform* t) { m_actor->SetUserTransform(t); }

  inline double getXSize() { return m_xsize; }
  inline double getYSize() { return m_ysize; }

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

  //! The update here will be overwritten in later classes.
  virtual void update();
 protected:
  vtkPolyData* m_polyData;
  vtkPolyDataMapper* m_mapper;
  vtkActor* m_actor;
  vtkProperty* m_property;

  vtkTransform* m_transform;

  double m_xsize;
  double m_ysize;

  double m_origin[3];
  double m_midpoint[3];
  double m_opposite[3];
  double m_pt1[3];
  double m_pt2[3];
};

#endif // RTPOLYDATA2D_H
