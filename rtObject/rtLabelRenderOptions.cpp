#include "rtLabelRenderOptions.h"
#include <iostream>

rtLabelRenderOptions::rtLabelRenderOptions() {
  setType(rtConstants::OT_TextLabel);
  setupGUI();
}

rtLabelRenderOptions::~rtLabelRenderOptions() {
  cleanupGUI();
}

//! Function called every time the apply button is pressed. 
void rtLabelRenderOptions::apply() {
  
}

//! Function called when new data is available. 
void rtLabelRenderOptions::update() {
  
}

//! Create the elements for the GUI
void rtLabelRenderOptions::setupGUI() {
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

void rtLabelRenderOptions::cleanupGUI() {

  if (m_vbox) delete m_vbox;
  if (m_nameLabel) delete m_nameLabel;

  if (m_chooseTextLayout) delete m_chooseTextLayout;
  if (m_chooseTextWidget) delete m_chooseTextWidget;
  if (m_chooseTextLabel) delete m_chooseTextLabel;
  if (m_chooseTextEdit) delete m_chooseTextEdit;
}
