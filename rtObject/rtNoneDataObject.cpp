#include "rtNoneDataObject.h"

//! Constructor
rtNoneDataObject::rtNoneDataObject() {
  setObjectType(rtConstants::OT_None);
  setupGUI();
}

//! Destructor
rtNoneDataObject::~rtNoneDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
/*!
  @todo Write the GUI
 */
void rtNoneDataObject::apply() {

}

//! Send the info to the GUI
/*!
  @todo Write the GUI
 */
void rtNoneDataObject::update() {

}

//! Set the GUI widgets.
/*!
  @todo Write the GUI
 */
void rtNoneDataObject::setupGUI() {

}

//! Clean the GUI widgets.
/*!
  @todo Write the GUI
 */
void rtNoneDataObject::cleanupGUI() {

}
