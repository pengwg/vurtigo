#include "rtRenderOptions.h"

rtRenderOptions::rtRenderOptions() {
  m_applyButton = new QPushButton("Apply");
}

rtRenderOptions::~rtRenderOptions() {
  if (m_applyButton) delete m_applyButton;
}


