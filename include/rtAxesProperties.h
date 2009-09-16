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
#ifndef RT_AXES_PROPERTIES_H
#define RT_AXES_PROPERTIES_H

#include "ui_axesProperties.h"

class rtAxesProperties : public QDialog, private Ui::axesProperties {
Q_OBJECT

 public:

  enum ViewType {
    VT_NONE,
    VT_VISIBLE,
    VT_INTERACT
  };

  enum CoordType {
    CT_PATIENT,
    CT_VTK
  };

  rtAxesProperties(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtAxesProperties();

  bool isChanged() { return m_changed; }

  ViewType getViewType() { return m_viewType; }
  CoordType getCoordType() { return m_coordType; }

 public slots:
  void settingsChanged();

 protected:
  //! True if the user clicked the OK button to apply the changes.
  bool m_changed;

  ViewType m_viewType;
  CoordType m_coordType;

};

#endif
