#ifndef RT_PIECE_FUNC_DATA_OBJECT_H
#define RT_PIECE_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"
#include "VtkPiecewiseGraph.h"

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

  void update();

  //! Get the value of the current piecewise function.
  vtkPiecewiseFunction* getPiecewiseFunction() {
    m_pieceFunc = m_graph->getPiecewiseFunction();
    return m_pieceFunc;
  }

  bool setPiecewiseFunction(vtkPiecewiseFunction* piece);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkPiecewiseFunction> m_pieceFunc;

  VtkPiecewiseGraph* m_graph;
  QBoxLayout *m_mainLayout;
};

#endif 
