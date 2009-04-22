#include "rt3dVolumeDataObject.h"

//! Constructor
rt3DVolumeDataObject::rt3DVolumeDataObject() {
  setObjectType(rtConstants::OT_3DObject);
  setupGUI();
}

//! Destructor
rt3DVolumeDataObject::~rt3DVolumeDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rt3DVolumeDataObject::apply() {

}

//! Send the info to the GUI
void rt3DVolumeDataObject::update() {

}

//! Set the GUI widgets.
void rt3DVolumeDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt3DVolumeDataObject::cleanupGUI() {

}
