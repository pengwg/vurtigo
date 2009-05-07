#include "rtAxesProperties.h"

rtAxesProperties::rtAxesProperties(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));

  m_changed=false;
  m_viewType = VT_VISIBLE;
  m_coordType = CT_PATIENT;

  visibleComboBox->setCurrentIndex(1);
  coordComboBox->setCurrentIndex(0);
}

//! Destructor
rtAxesProperties::~rtAxesProperties() {
}

//! The user has clicked on OK so the changes will be made.
void rtAxesProperties::settingsChanged() {
  m_changed=true;
  int vt, ct;

  vt =  visibleComboBox->currentIndex();
  ct = coordComboBox->currentIndex();

  switch (vt) {
  case 0:
    m_viewType = VT_NONE;
    break;
  case 1:
    m_viewType = VT_VISIBLE;
    break;
  case 2:
    m_viewType = VT_INTERACT;
    break;  
  }

  switch (ct) {
  case 0:
    m_coordType = CT_PATIENT;
    break;
  case 1:
    m_coordType = CT_VTK;
    break;
  }

}
