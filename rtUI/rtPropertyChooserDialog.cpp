#include "rtPropertyChooserDialog.h"

//! Constructor to the dialog
rtPropertyChooserDialog::rtPropertyChooserDialog(vtkProperty* initial, QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));

  m_changed=false;
  m_prop = NULL;

  if (initial) {
    m_prop = initial;

    representComboBox->setCurrentIndex(m_prop->GetRepresentation());

  } else {
    std::cout << "Error: rtPropertyChooserDialog.cpp: Initial Property was NULL!" << std::endl;
  }

}

rtPropertyChooserDialog::~rtPropertyChooserDialog() {

}

//! The user has clicked on OK so the changes to the vtkProperty will be made.
void rtPropertyChooserDialog::settingsChanged() {
  if (!m_prop) return;
  m_changed=true;

  m_prop->SetRepresentation( representComboBox->currentIndex() );
}
