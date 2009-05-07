#include "rt4dVolumeDataObject.h"

//! Constructor
rt4DVolumeDataObject::rt4DVolumeDataObject() {
  setObjectType(rtConstants::OT_4DObject);
  setupGUI();
}

//! Destructor
rt4DVolumeDataObject::~rt4DVolumeDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rt4DVolumeDataObject::apply() {

}

//! Send the info to the GUI
void rt4DVolumeDataObject::update() {

}

//! Set the GUI widgets.
void rt4DVolumeDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt4DVolumeDataObject::cleanupGUI() {

}
