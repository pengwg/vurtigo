#ifndef RTCROSSHAIR2D_H
#define RTCROSSHAIR2D_H

#include "rtPolyData2D.h"

//! Create a pipeline and final actor for a set of crosshairs.
class rtCrosshair2D : public rtPolyData2D
{
public:
  rtCrosshair2D();
  ~rtCrosshair2D();

  virtual void update();
protected:
  void findMidpoint(double in1[3], double in2[3], double out[3]);
};

#endif // RTCROSSHAIR2D_H
