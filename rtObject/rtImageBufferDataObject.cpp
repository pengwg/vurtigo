#include "rtImageBufferDataObject.h"

//! Constructor
rtImageBufferDataObject::rtImageBufferDataObject() {
  setObjectType(rtConstants::OT_ImageBuffer);
  setupGUI();
}

//! Destructor
rtImageBufferDataObject::~rtImageBufferDataObject() {
  cleanupGUI();
}

//! Send the info to the GUI
void rtImageBufferDataObject::update() {

}

//! Set the GUI widgets.
void rtImageBufferDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtImageBufferDataObject::cleanupGUI() {

}
