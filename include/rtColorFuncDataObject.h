#ifndef RT_COLOR_FUNC_DATA_OBJECT_H
#define RT_COLOR_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"

#include <vtkSmartPointer.h>
#include <vtkColorTransferFunction.h>

//! The data portion of the color function
/*!
  Holds the points that determine how scalars will be colored.
  */
class rtColorFuncDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtColorFuncDataObject();
  ~rtColorFuncDataObject();

  void apply();
  void update();

  //! Get the value of the current color function.
  vtkColorTransferFunction* getColorFunction() { return m_ctf; }
  bool setColorFunction(vtkColorTransferFunction* ctf);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkColorTransferFunction> m_ctf;
};

#endif 
