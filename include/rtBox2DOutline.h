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

// VTK
#include <vtkTransform.h>

// Local
#include "rtPolyData2D.h"

//! Creates the pipeline and the final actor for a set of four lines in the shape of a box.
class rtBox2DOutline : public rtPolyData2D
{
 public:
  //! Constructor
  rtBox2DOutline();

  //! Destructor
  ~rtBox2DOutline();

  //! Update the widget.
  virtual void update();

  //! Set the color of the outline
  /*!
    Set the color as rgb. The range for each value is in [0, 1].
    \param r The red component [0,1]
    \param g The green component [0,1]
    \param b The blue component [0,1]
    */
  void setOutlineColor(double r, double g, double b);

 protected:

};

#endif // RTBOX2DOUTLINE_H
