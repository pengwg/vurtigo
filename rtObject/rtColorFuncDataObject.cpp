#include "rtColorFuncDataObject.h"

//! Constructor
rtColorFuncDataObject::rtColorFuncDataObject() {
  setObjectType(rtConstants::OT_vtkColorTransferFunction);
  m_ctf = vtkColorTransferFunction::New();
  setupGUI();
}

//! Destructor
rtColorFuncDataObject::~rtColorFuncDataObject() {
  cleanupGUI();
  m_ctf->Delete();
}

//! Set a new color funciton.
bool rtColorFuncDataObject::setColorFunction(vtkColorTransferFunction* ctf) {
  if (!ctf) return false;
  m_ctf->DeepCopy(ctf);
  return true;
}

//! Take the info from the GUI
void rtColorFuncDataObject::apply() {

}

//! Send the info to the GUI
void rtColorFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtColorFuncDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtColorFuncDataObject::cleanupGUI() {

}
