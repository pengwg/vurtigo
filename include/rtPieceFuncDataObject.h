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
#ifndef RT_PIECE_FUNC_DATA_OBJECT_H
#define RT_PIECE_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"
#include "VtkPiecewiseGraph.h"

#include <vtkSmartPointer.h>
#include <vtkPiecewiseFunction.h>

class QXmlStreamReader;

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

  //! Send the info to the GUI
  void update();

  //! Get the value of the current piecewise function.
  vtkPiecewiseFunction* getPiecewiseFunction() {
    m_pieceFunc->DeepCopy(m_graph->getPiecewiseFunction());
    return m_pieceFunc;
  }

  bool setPiecewiseFunction(vtkPiecewiseFunction* piece);

  virtual bool saveFile(QFile *file);
  virtual bool loadFile(QFile *file);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkPiecewiseFunction> m_pieceFunc;

  VtkPiecewiseGraph* m_graph;
  QBoxLayout *m_mainLayout;

  void readFileInfo(QXmlStreamReader* reader);
  void readFuncData(QXmlStreamReader* reader);
  void readPointData(QXmlStreamReader* reader);
};

#endif 
