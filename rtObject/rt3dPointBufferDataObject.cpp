#include "rt3dPointBufferDataObject.h"

//! Constructor
rt3DPointBufferDataObject::rt3DPointBufferDataObject() {
  m_pointList.clear();
  setObjectType(rtConstants::OT_3DPointBuffer);
  setupGUI();
}

//! Destructor
rt3DPointBufferDataObject::~rt3DPointBufferDataObject() {
  m_pointList.clear();
  cleanupGUI();
}

//! Add a point to the list
void rt3DPointBufferDataObject::addPoint(SimplePoint sp) {
  m_pointList.append(sp);
  Modified();
}

//! Remove a point from the list
void rt3DPointBufferDataObject::removePoint(SimplePoint sp) {
  int i;

  if (m_pointList.contains(sp)) {
    i = m_pointList.indexOf(sp);
    m_pointList.removeAt(i);
    Modified();
  }
}


//! Take the info from the GUI
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::apply() {

}

//! Send the info to the GUI
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::update() {

}

//! Set the GUI widgets.
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::setupGUI() {

}

//! Clean the GUI widgets.
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::cleanupGUI() {

}
