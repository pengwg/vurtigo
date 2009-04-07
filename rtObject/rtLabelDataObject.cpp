#include "rtLabelDataObject.h"
#include "rtLabelRenderOptions.h"

rtLabelDataObject::rtLabelDataObject() {
  setObjectType(rtConstants::OT_TextLabel);
  m_labelText = "Sample Text";

  m_textProp = vtkTextProperty::New();
}

rtLabelDataObject::~rtLabelDataObject() {
  if (m_textProp) m_textProp->Delete();
}

//! Take the info from the GUI
void rtLabelDataObject::apply() {
  rtLabelRenderOptions* tempRenderOpt;
  tempRenderOpt = dynamic_cast<rtLabelRenderOptions*>(getRenderOptions());
  m_labelText = tempRenderOpt->getChooseTextEdit()->text();
}

//! Send the info to the GUI
void rtLabelDataObject::update() {
  rtLabelRenderOptions* tempRenderOpt;
  tempRenderOpt = dynamic_cast<rtLabelRenderOptions*>(getRenderOptions());
  tempRenderOpt->getChooseTextEdit()->setText(m_labelText);
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
