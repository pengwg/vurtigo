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
#ifndef RT_COLOR_FUNC_DATA_OBJECT_H
#define RT_COLOR_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"
#include "VtkColorTable.h"

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

  void update();

  //! Get the value of the current color function.
  vtkColorTransferFunction* getColorFunction() {
    m_ctf->DeepCopy(m_table->getColorFunction());
    return m_ctf;
  }
  bool setColorFunction(vtkColorTransferFunction* ctf);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkColorTransferFunction> m_ctf;

  VtkColorTable* m_table;
  QBoxLayout *m_mainLayout;
};

#endif 
