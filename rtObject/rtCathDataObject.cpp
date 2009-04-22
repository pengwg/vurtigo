#include "rtCathDataObject.h"

//! Constructor
rtCathDataObject::rtCathDataObject() {
  setObjectType(rtConstants::OT_Cath);
  setupGUI();
}

//! Destructor
rtCathDataObject::~rtCathDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rtCathDataObject::apply() {

}

//! Send the info to the GUI
void rtCathDataObject::update() {

}

//! Set the GUI widgets.
void rtCathDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtCathDataObject::cleanupGUI() {

}
