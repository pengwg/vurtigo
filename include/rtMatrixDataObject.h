#ifndef RT_MATRIX_DATA_OBJECT_H
#define RT_MATRIX_DATA_OBJECT_H

#include "rtDataObject.h"

#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>

//! Matrix Object
/*!
  A simple vtkTransform object that can be used to rotate/translate graphics objects.
  */
class rtMatrixDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtMatrixDataObject();
  ~rtMatrixDataObject();

  void apply();
  void update();

  //! Return the transform as a matrix object.
  vtkMatrix4x4* getMatrix() { return m_currTransform->GetMatrix(); }

  //! Return the transform for use in VTK.
  vtkTransform* getTransform() { return m_currTransform; }

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  vtkSmartPointer<vtkTransform> m_currTransform;
  
};

#endif 
