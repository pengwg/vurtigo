#include "rtNoneDataObject.h"

//! Constructor
rtNoneDataObject::rtNoneDataObject() {
  setObjectType("OT_None");
  setupGUI();
}

//! Destructor
rtNoneDataObject::~rtNoneDataObject() {
  cleanupGUI();
}

//! Send the info to the GUI
void rtNoneDataObject::update() {

}

//! Set the GUI widgets.
void rtNoneDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtNoneDataObject::cleanupGUI() {

}
