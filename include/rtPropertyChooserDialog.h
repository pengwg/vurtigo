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
#ifndef RT_PROPERTY_CHOOSER_DIALOG_H
#define RT_PROPERTY_CHOOSER_DIALOG_H

#include "ui_propertyChooserDialog.h"
#include "vtkProperty.h"
#include <QAbstractButton>

class rtPropertyChooserDialog : public QDialog, private Ui::propertyChooserDialog {

Q_OBJECT
  
  public:
    rtPropertyChooserDialog(vtkProperty* initial, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~rtPropertyChooserDialog();

    bool isChanged() { return m_changed; }
 public slots:
  void settingsChanged();
  void buttonClicked(QAbstractButton*);

  void changeRenderMode();

  void changeSurfaceColor();
  void changeEdgeVisibility(bool);
  void changeEdgeColor();

  void changeSurfaceSpinBox();
  void changeEdgeSpinBox();

  void changeOpacity(int);
  void changeAmbient(int);
  void changeDiffuse(int);
  void changeSpecular(int);
  void changeSpecularPower(int);



signals:
  void propertyChanged();

  protected:
    vtkProperty* m_prop;

    //! True if the user clicked the OK button to apply the changes.
    bool m_changed;
};

#endif
