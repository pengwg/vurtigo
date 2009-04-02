#include "rtLabelDataObject.h"

rtLabelDataObject::rtLabelDataObject() {
  setObjectType(rtConstants::OT_TextLabel);
  m_labelText = "";

  m_textProp = vtkTextProperty::New();
}

rtLabelDataObject::~rtLabelDataObject() {
  if (m_textProp) m_textProp->Delete();
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
