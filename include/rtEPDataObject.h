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
#ifndef RT_EP_DATA_OBJECT_H
#define RT_EP_DATA_OBJECT_H

#include "rtDataObject.h"

#include <QList>
#include <QTimer>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkColorTransferFunction.h>

//! Object that represents electro phisiological data
/*!
  EP Data object.
  */
class rtEPDataObject : public rtDataObject
{
Q_OBJECT

public:

  rtEPDataObject();
  ~rtEPDataObject();

  void update();

  bool setCurrTrigDelay(int trigDelay);
  bool setCurrPhase(int phase);
  
  int getCurrPhase() { return m_currentPhase; }

  virtual bool saveFile(QFile *file);
  virtual bool loadFile(QFile *file);
 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  int m_currentPhase;
};

#endif 
