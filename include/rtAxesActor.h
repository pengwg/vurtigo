#ifndef RTAXESACTOR_H
#define RTAXESACTOR_H

#include <vtkAxesActor.h>

class rtAxesActor : public vtkAxesActor
{
public:
  static rtAxesActor *New();

  // Overloaded to make the axes actor orientable.
  // The vtkAxesActor does not compute its bounds in a way that
  // allows it to be re-oriented.
  double *GetBounds();
};

#endif // RTAXESACTOR_H
