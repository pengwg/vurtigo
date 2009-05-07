#include "rtPolyDataObject.h"

//! Constructor
rtPolyDataObject::rtPolyDataObject() {
  setObjectType(rtConstants::OT_vtkPolyData);
  setupGUI();
}

//! Destructor
rtPolyDataObject::~rtPolyDataObject() {
  cleanupGUI();
}


//! Take the info from the GUI
void rtPolyDataObject::apply() {

}

//! Send the info to the GUI
void rtPolyDataObject::update() {

}

//! Set the GUI widgets.
void rtPolyDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtPolyDataObject::cleanupGUI() {

}
