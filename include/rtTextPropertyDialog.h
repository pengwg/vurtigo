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
#ifndef RT_TEXT_PROPERTY_DIALOG_H
#define RT_TEXT_PROPERTY_DIALOG_H

#include "ui_textPropertyDialog.h"
#include "vtkTextProperty.h"

class rtTextPropertyDialog : public QDialog, private Ui::textPropertyDialog {
  Q_OBJECT
 
 public:
  rtTextPropertyDialog(vtkTextProperty* initial, QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtTextPropertyDialog();

  //! Check if the property was changed.
  bool isChanged() { return m_changed; }

  //! Get the newly changed property.
  vtkTextProperty* getProperty() { return m_textProp; }

 public slots:
  void settingsChanged();
  void colorButton();

 protected:
  //! True if the user clicked the OK button to apply the changes.
  bool m_changed;

  //! The text property to be modified. 
  vtkTextProperty* m_textProp;

};

#endif
