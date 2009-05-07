#include "rtMatrixDataObject.h"

//! Constructor
rtMatrixDataObject::rtMatrixDataObject() {
  setObjectType(rtConstants::OT_vtkMatrix4x4);

  m_currMatrix= vtkMatrix4x4::New();

  setupGUI();
}

//! Destructor
rtMatrixDataObject::~rtMatrixDataObject() {
  cleanupGUI();

  if (m_currMatrix) m_currMatrix->Delete();
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
