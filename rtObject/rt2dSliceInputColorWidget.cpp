/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "rt2dSliceInputColorWidget.h"
#include "rtApplication.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "rtDataObject.h"

rt2DSliceInputColorWidget::rt2DSliceInputColorWidget(int sliceObjectID)
    : m_sliceObjectID(sliceObjectID), m_updatingLists(false), m_currentObjectID(-1)
{
  m_colorButton.setText("Change Color...");
  m_currentColor = Qt::white;

  // Layout
  m_widgetLayout.addWidget(&m_itemName);
  m_widgetLayout.addWidget(&m_colorButton);

  this->setLayout(&m_widgetLayout);

  // Signals and Slots
  connect(&m_itemName, SIGNAL(currentIndexChanged(int)), this, SLOT(itemIndexChanged(int)));
  connect(&m_colorButton, SIGNAL(clicked()), this, SLOT(colorButtonPressed()));

  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(newVurtigoObject(int)));
  connect(rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(removedVurtigoObject(int)));

  fillDropdownList();
}

rt2DSliceInputColorWidget::~rt2DSliceInputColorWidget() {
}

void rt2DSliceInputColorWidget::getCurrentColorAsVTKArray(double color[3]) {
  color[0] = m_currentColor.redF();
  color[1] = m_currentColor.greenF();
  color[2] = m_currentColor.blueF();
}

void rt2DSliceInputColorWidget::setCurrentObjectID(int id) {
  // Check if it is already set
  if (m_currentObjectID == id) return;

  QList<int> keys = m_dropdownToObjectId.keys(id);
  if (keys.size() > 0) {
    m_currentObjectID = id;
    m_itemName.setCurrentIndex(keys[0]);
    emit inputIdModified(m_currentObjectID);
  }
}

//////////////////
// Public Slots
//////////////////
void rt2DSliceInputColorWidget::itemIndexChanged(int newIndex) {
  if (newIndex == -1 || m_updatingLists) return;

  if (m_dropdownToObjectId.value(newIndex) != m_currentObjectID) {
    m_currentObjectID = m_dropdownToObjectId.value(newIndex);
    emit inputIdModified(m_currentObjectID);
  }
}

void rt2DSliceInputColorWidget::colorButtonPressed() {
  QColor newColor;
  newColor = QColorDialog::getColor(m_currentColor, this);
  if (newColor.isValid() && m_currentColor!=newColor) {
    m_currentColor=newColor;
    emit inputColorModified(m_currentColor);
  }
}

void rt2DSliceInputColorWidget::newVurtigoObject(int objID) {
  rtRenderObject* tempR;
  rtDataObject* tempD;
  int loc;

  tempR = rtApplication::instance().getObjectManager()->getObjectWithID(objID);
  if (tempR) {
    if (tempR->getObjectType() == "OT_3DObject") {
      tempD = tempR->getDataObject();
      loc = m_dropdownToObjectId.size();
      m_dropdownToObjectId.insert(loc, objID);
      m_itemName.insertItem(loc, tempD->getObjName());
    }
  }
}

void rt2DSliceInputColorWidget::removedVurtigoObject(int objID) {
  QList<int> keys = m_dropdownToObjectId.keys(objID);
  if (keys.size() > 0) {
    // For now, just reset the lists...
    // However, there is probably a smarter way to do this that only removes the single element.
    fillDropdownList();
  }
}

//////////////
// Protected
//////////////
void rt2DSliceInputColorWidget::fillDropdownList() {
  rtRenderObject* tempR;
  rtDataObject* tempD;

  int originalIndex = m_itemName.currentIndex();
  if (originalIndex < 0) originalIndex = 0;

  // Start updating the lists.
  m_updatingLists = true;

  m_dropdownToObjectId.clear();
  m_itemName.clear();

  // Insert the none.
  m_dropdownToObjectId.insert(0, -1);
  m_itemName.insertItem(0, "None");

  // Insert the current object.
  m_dropdownToObjectId.insert(1, m_sliceObjectID);
  m_itemName.insertItem(1, "Real-Time 2D Input");

  // Get a list of all the 3D/4D type objects.
  QList<int> volumeObjects = rtApplication::instance().getObjectManager()->getObjectsOfType("OT_3DObject");
  for (int ix1=0; ix1<volumeObjects.size(); ix1++) {
    tempR = rtApplication::instance().getObjectManager()->getObjectWithID(volumeObjects.at(ix1));
    if (tempR) {
      tempD = tempR->getDataObject();
      m_dropdownToObjectId.insert(2+ix1, volumeObjects.at(ix1));
      m_itemName.insertItem(2+ix1, tempD->getObjName());
    }
  }

  // Finished updating the lists.
  m_updatingLists = false;

  // Restore the original index.
  if (originalIndex >= m_itemName.count()) {
    originalIndex = m_itemName.count()-1;
  }
  m_itemName.setCurrentIndex(originalIndex);

}
