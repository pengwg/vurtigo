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

void rt2DPointDataObject::addPoint(SimplePoint2D sp) {
  m_pointList.append(sp);
  Modified();
}

void rt2DPointDataObject::removePoint(SimplePoint2D sp) {
  int i;

  if (m_pointList.contains(sp)) {
    i = m_pointList.indexOf(sp);
    m_pointList.removeAt(i);
    Modified();
  }
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
