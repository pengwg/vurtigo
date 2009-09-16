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
#include "rtAxesProperties.h"

rtAxesProperties::rtAxesProperties(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));

  m_changed=false;
  m_viewType = VT_VISIBLE;
  m_coordType = CT_PATIENT;

  visibleComboBox->setCurrentIndex(1);
  coordComboBox->setCurrentIndex(0);
}

//! Destructor
rtAxesProperties::~rtAxesProperties() {
}

//! The user has clicked on OK so the changes will be made.
void rtAxesProperties::settingsChanged() {
  m_changed=true;
  int vt, ct;

  vt =  visibleComboBox->currentIndex();
  ct = coordComboBox->currentIndex();

  switch (vt) {
  case 0:
    m_viewType = VT_NONE;
    break;
  case 1:
    m_viewType = VT_VISIBLE;
    break;
  case 2:
    m_viewType = VT_INTERACT;
    break;  
  }

  switch (ct) {
  case 0:
    m_coordType = CT_PATIENT;
    break;
  case 1:
    m_coordType = CT_VTK;
    break;
  }

}
