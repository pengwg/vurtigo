#include "rtLabelDataObject.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

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

  m_vbox = new QVBoxLayout();
  wid->setLayout(m_vbox);

  m_nameLabel = new QLabel("Text Label Options:", wid);
  m_vbox->addWidget(m_nameLabel);

  m_chooseTextWidget = new QWidget();
  m_chooseTextLayout = new QHBoxLayout();
  m_chooseTextWidget->setLayout(m_chooseTextLayout);
  m_vbox->addWidget(m_chooseTextWidget);
  m_chooseTextLabel = new QLabel("Display Text: ");
  m_chooseTextEdit = new QLineEdit();

  m_chooseTextLayout->addWidget(m_chooseTextLabel);
  m_chooseTextLayout->addWidget(m_chooseTextEdit);

  m_vbox->addWidget(getApplyButton());
}

void rtLabelDataObject::cleanupGUI() {

  if (m_vbox) delete m_vbox;
  if (m_nameLabel) delete m_nameLabel;

  if (m_chooseTextLayout) delete m_chooseTextLayout;
  if (m_chooseTextWidget) delete m_chooseTextWidget;
  if (m_chooseTextLabel) delete m_chooseTextLabel;
  if (m_chooseTextEdit) delete m_chooseTextEdit;
}

//! Take the info from the GUI
void rtLabelDataObject::apply() {
  m_labelText = m_chooseTextEdit->text();
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
