#include "rtPieceFuncDataObject.h"

//! Constructor
rtPieceFuncDataObject::rtPieceFuncDataObject() {
  setObjectType(rtConstants::OT_vtkPiecewiseFunction);
  setupGUI();
}

//! Destructor
rtPieceFuncDataObject::~rtPieceFuncDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rtPieceFuncDataObject::apply() {

}

//! Send the info to the GUI
void rtPieceFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtPieceFuncDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtPieceFuncDataObject::cleanupGUI() {

}
