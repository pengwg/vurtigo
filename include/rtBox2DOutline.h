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
