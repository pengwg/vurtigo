/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "rtLabelDataObject.h"
#include "rtTextPropertyDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QColorDialog>

rtLabelDataObject::rtLabelDataObject() {
  setObjectType("OT_TextLabel");
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
