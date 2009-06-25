#include "rtMatrixDataObject.h"

//! Constructor
rtMatrixDataObject::rtMatrixDataObject() {
  setObjectType(rtConstants::OT_vtkMatrix4x4);

  m_currTransform = vtkTransform::New();

  setupGUI();
}

//! Destructor
rtMatrixDataObject::~rtMatrixDataObject() {
  cleanupGUI();

  if (m_currTransform) m_currTransform->Delete();
}


//! Take the info from the GUI
void rtMatrixDataObject::apply() {

}

//! Send the info to the GUI
void rtMatrixDataObject::update() {

}

//! Set the GUI widgets.
void rtMatrixDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtMatrixDataObject::cleanupGUI() {

}
