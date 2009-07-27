#ifndef RT_PIECE_FUNC_DATA_OBJECT_H
#define RT_PIECE_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"

#include <vtkSmartPointer.h>
#include <vtkPiecewiseFunction.h>

//! Data Object from Piecewise Function
/*!
  The Piecewise Function data defines the opacity of different scalar values.
  */
class rtPieceFuncDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtPieceFuncDataObject();
  ~rtPieceFuncDataObject();

  void apply();
  void update();

  //! Get the value of the current piecewise function.
  vtkPiecewiseFunction* getPiecewiseFunction() { return m_pieceFunc; }
  bool setPiecewiseFunction(vtkPiecewiseFunction* piece);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkPiecewiseFunction> m_pieceFunc;
};

#endif 
