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
#include "rtColorFuncDataObject.h"

//! Constructor
rtColorFuncDataObject::rtColorFuncDataObject() {
  setObjectType(rtConstants::OT_vtkColorTransferFunction);
  m_ctf = vtkColorTransferFunction::New();
  setupGUI();
}

//! Destructor
rtColorFuncDataObject::~rtColorFuncDataObject() {
  cleanupGUI();
  m_ctf->Delete();
}

//! Set a new color funciton.
bool rtColorFuncDataObject::setColorFunction(vtkColorTransferFunction* ctf) {
  if (!ctf) return false;
  m_ctf->DeepCopy(ctf);
  m_table->setColorFunction(m_ctf);
  return true;
}

//! Send the info to the GUI
void rtColorFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtColorFuncDataObject::setupGUI() {
  m_table = new VtkColorTable();
  QWidget *wid = getBaseWidget();
  m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, wid);

  connect(m_table, SIGNAL(functionUpdated()), this, SLOT(Modified()));

  wid->setLayout(m_mainLayout);
  m_mainLayout->addWidget(m_table);
}

//! Clean the GUI widgets.
void rtColorFuncDataObject::cleanupGUI() {
  m_mainLayout->removeWidget(m_table);
  delete m_mainLayout;
  delete m_table;
}
