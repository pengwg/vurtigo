#include "rtLabelRenderOptions.h"

rtLabelRenderOptions::rtLabelRenderOptions() {
  setType(rtConstants::OT_TextLabel);
  setupGUI();
}

rtLabelRenderOptions::~rtLabelRenderOptions() {
  cleanupGUI();
}


void rtLabelRenderOptions::setupGUI() {
  QWidget* wid = getBaseWidget();

  m_vbox = new QVBoxLayout();
  wid->setLayout(m_vbox);

  m_nameLabel = new QLabel("Text Label Options:", wid);
  m_vbox->addWidget(m_nameLabel);
  
}

void rtLabelRenderOptions::cleanupGUI() {

  if (m_vbox) delete m_vbox;
  if (m_nameLabel) delete m_nameLabel;

}
