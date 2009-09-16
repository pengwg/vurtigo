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
#include "rt3dPointBufferDataObject.h"

//! Constructor
rt3DPointBufferDataObject::rt3DPointBufferDataObject() {
  m_pointList.clear();
  setObjectType(rtConstants::OT_3DPointBuffer);
  setupGUI();
}

//! Destructor
rt3DPointBufferDataObject::~rt3DPointBufferDataObject() {
  m_pointList.clear();
  cleanupGUI();
}

//! Add a point to the list
void rt3DPointBufferDataObject::addPoint(SimplePoint sp) {
  m_pointList.append(sp);
  Modified();
}

//! Remove a point from the list
void rt3DPointBufferDataObject::removePoint(SimplePoint sp) {
  int i;

  if (m_pointList.contains(sp)) {
    i = m_pointList.indexOf(sp);
    m_pointList.removeAt(i);
    Modified();
  }
}


//! Send the info to the GUI
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::update() {

}

//! Set the GUI widgets.
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::setupGUI() {

}

//! Clean the GUI widgets.
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::cleanupGUI() {

}
