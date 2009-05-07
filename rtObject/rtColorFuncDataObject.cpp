#include "rtColorFuncDataObject.h"

//! Constructor
rtColorFuncDataObject::rtColorFuncDataObject() {
  setObjectType(rtConstants::OT_vtkColorTransferFunction);
  setupGUI();
}

//! Destructor
rtColorFuncDataObject::~rtColorFuncDataObject() {
  cleanupGUI();
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
