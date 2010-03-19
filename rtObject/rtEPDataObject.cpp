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
#include "rtEPDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

#include <QHash>
#include <QColor>
#include <QString>

#include <vtkPoints.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

//! Constructor
rtEPDataObject::rtEPDataObject() : m_currentPhase(-1) {
  setObjectType(rtConstants::OT_EPMesh);

  setupGUI();
}

//! Destructor
rtEPDataObject::~rtEPDataObject() {
  cleanupGUI();
}
  

  
bool rtEPDataObject::setCurrTrigDelay(int trigDelay) {

  return true;
}

bool rtEPDataObject::setCurrPhase(int phase) {

  return true;
}
  
//! Send the info to the GUI
void rtEPDataObject::update() {

}

bool rtEPDataObject::saveFile(QFile *file) {
  if (!file->open(QIODevice::WriteOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for writing. Error Code: ").append(QString::number(file->error())));
    return false;
  }


  file->close();
  return true;
}

bool rtEPDataObject::loadFile(QFile *file) {
  if (!file->open(QIODevice::ReadOnly)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for reading. Error Code: ").append(QString::number(file->error())));
    return false;
  }


  Modified();
  return true;
}

////////////////
// Protected
////////////////
//! Set the GUI widgets.
void rtEPDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtEPDataObject::cleanupGUI() {

}
