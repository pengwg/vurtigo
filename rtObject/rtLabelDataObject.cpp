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
#include "rtLabelDataObject.h"
#include "rtTextPropertyDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QColorDialog>

rtLabelDataObject::rtLabelDataObject() {
  setObjectType(rtConstants::OT_TextLabel);
  m_labelText = "Sample Text";
  m_textProp = vtkTextProperty::New();
  setupGUI();
}

rtLabelDataObject::~rtLabelDataObject() {
  if (m_textProp) m_textProp->Delete();
  cleanupGUI();
}

//! Create the elements for the GUI
void rtLabelDataObject::setupGUI() {
  QWidget* wid = getBaseWidget();

  m_masterLayout = new QGridLayout();
  wid->setLayout(m_masterLayout);

  m_nameLabel = new QLabel("Text Label Options:", wid);
  m_masterLayout->addWidget(m_nameLabel, 0, 0);

  m_chooseTextLabel.setText("Display Text: ");
  m_chooseTextEdit = new QLineEdit();
  connect(m_chooseTextEdit, SIGNAL(textEdited(QString)), this, SLOT(textChanged(QString)));
  m_masterLayout->addWidget(&m_chooseTextLabel, 1, 0);
  m_masterLayout->addWidget(m_chooseTextEdit, 1, 1);

  m_propertyLabel.setText("Text Options: ");
  m_propertyButton.setText("Render Options");
  connect(&m_propertyButton, SIGNAL(pressed()), this, SLOT(propertyDialog()));

  m_masterLayout->addWidget(&m_propertyLabel, 2, 0);
  m_masterLayout->addWidget(&m_propertyButton, 2, 1);

}

void rtLabelDataObject::cleanupGUI() {

  if (m_masterLayout) delete m_masterLayout;
  if (m_nameLabel) delete m_nameLabel;

  if (m_chooseTextEdit) delete m_chooseTextEdit;
}

//! Take the info from the GUI
void rtLabelDataObject::textChanged(QString txt) {
  m_labelText = txt;
  Modified();
}

//! Send the info to the GUI
void rtLabelDataObject::update() {
  m_chooseTextEdit->setText(m_labelText);
}

void rtLabelDataObject::setText(QString text) {
  m_labelText = text;
}

QString rtLabelDataObject::getText() {
  return m_labelText;
}


void rtLabelDataObject::setColor(double r, double g, double b) {
  if (m_textProp) {
    m_textProp->SetColor(r, g, b);
  }
}
  
void rtLabelDataObject::setOpacity(double opac) {
  if (m_textProp) {
    m_textProp->SetOpacity(opac);
  }
}

vtkTextProperty* rtLabelDataObject::getTextProperty() {
  return m_textProp;
}

//! Modify text properties
void rtLabelDataObject::propertyDialog() {
  rtTextPropertyDialog textDlg(m_textProp);
  textDlg.exec();
  if (textDlg.isChanged()) Modified();
}
