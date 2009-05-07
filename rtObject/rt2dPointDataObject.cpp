#include "rt2dPointDataObject.h"

//! Constructor
rt2DPointDataObject::rt2DPointDataObject() {
  setObjectType(rtConstants::OT_2DPointBuffer);
  setupGUI();
}

//! Destructor
rt2DPointDataObject::~rt2DPointDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rt2DPointDataObject::apply() {

}

//! Send the info to the GUI
void rt2DPointDataObject::update() {

}

//! Set the GUI widgets.
void rt2DPointDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rt2DPointDataObject::cleanupGUI() {

}
