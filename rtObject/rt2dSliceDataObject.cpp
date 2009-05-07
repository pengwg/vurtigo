#include "rt2dSliceDataObject.h"

//! Constructor
rt2DSliceDataObject::rt2DSliceDataObject() {
  setObjectType(rtConstants::OT_2DObject);
  setupGUI();
}

//! Destructor
rt2DSliceDataObject::~rt2DSliceDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rt2DSliceDataObject::apply() {

}

//! Send the info to the GUI
void rt2DSliceDataObject::update() {

}

//! Set the GUI widgets.
void rt2DSliceDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt2DSliceDataObject::cleanupGUI() {

}
