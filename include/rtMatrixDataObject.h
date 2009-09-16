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
