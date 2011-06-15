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

#include "ObjectSelectionComboBox.h"
#include "rtRenderObject.h"
#include "rtDataObject.h"

ObjectSelectionComboBox::ObjectSelectionComboBox()
 : m_currentObjectIndex(-1)
{
  m_objectTypes.clear();
  this->clear();
  this->addItem("No Object Selected", -1);
  rtObjectManager* objManagerHandle = rtApplication::instance().getObjectManager();

  connect( objManagerHandle, SIGNAL(objectCreated(int)), this, SLOT(objectCreated(int)) );
  connect( objManagerHandle, SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );
  connect( objManagerHandle, SIGNAL(objectRenamed(int)), this, SLOT(objectRenamed(int)) );
  connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(newSelection(int)) );
}


void ObjectSelectionComboBox::addObjectType(QString type) {
  if (!m_objectTypes.contains(type)) {
    m_objectTypes.append(type);
    updateComboList();
  }
}

void ObjectSelectionComboBox::removeObjectType(QString type) {
  if (m_objectTypes.contains(type)) {
    m_objectTypes.removeAll(type);
    updateComboList();
  }
}

////////////////////
// Public Slots
////////////////////
void ObjectSelectionComboBox::objectCreated(int id) {
  rtObjectManager* objManagerHandle = rtApplication::instance().getObjectManager();
  rtRenderObject* renderObj = objManagerHandle->getObjectWithID(id);
  if (renderObj) {
    if ( m_objectTypes.contains(renderObj->getObjectType()) ) {
        this->addItem(renderObj->getDataObject()->getObjName() + " " + QString::number(id), id);
    }
  }
}

void ObjectSelectionComboBox::objectRemoved(int id) {
  // Object is removed. Can no longer check for the type as the object manager has deleted that object.
  int loc;

  // Check the list to delete this ID if it exists.
  loc = this->findData(id);
  if (loc >= 0) {
    if (m_currentObjectIndex == id) {

      // Disconnect the signal from the slot.
      disconnect(this, SLOT(currObjectModified(int)));

      m_currentObjectIndex = -1;
      this->setCurrentIndex(this->findData(-1));
      emit objectSelectionChanged(m_currentObjectIndex);
    }
    this->removeItem(loc);
  }
}

void ObjectSelectionComboBox::objectRenamed(int id)
{
    disconnect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(newSelection(int)) );
    this->updateComboList();
    connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(newSelection(int)) );
}

void ObjectSelectionComboBox::newSelection(int sel) {
  int oldIndex = m_currentObjectIndex;
  rtObjectManager* objManagerHandle = rtApplication::instance().getObjectManager();
  rtRenderObject* renderObj;

  // Disconnect the signal from the slot.
  disconnect(this, SLOT(currObjectModified(int)));

  if ( sel >= 0 ) {
    m_currentObjectIndex = itemData(sel).toInt();
    if (m_currentObjectIndex != -1) {
      renderObj = objManagerHandle->getObjectWithID(m_currentObjectIndex);
      // Valid object so connect the signal
      connect(renderObj->getDataObject(), SIGNAL(objectChanged(int)), this, SLOT(currObjectModified(int)));
    }
  } else {
    m_currentObjectIndex = -1;
  }
  if (m_currentObjectIndex != oldIndex) emit objectSelectionChanged(m_currentObjectIndex);
}

void ObjectSelectionComboBox::currObjectModified(int id) {
  if (id != -1 && id == m_currentObjectIndex) {
    emit selectedObjectModified(m_currentObjectIndex);
  }
}

////////////////////
// Protected
////////////////////
void ObjectSelectionComboBox::updateComboList() {
  QList<int> tempObjList;
  rtObjectManager* objManagerHandle = rtApplication::instance().getObjectManager();
  rtRenderObject* renderObj;

  this->clear();
  this->addItem("No Object Selected", -1);

  for (int ix1=0; ix1<m_objectTypes.size(); ix1++) {
    tempObjList = objManagerHandle->getObjectsOfType(m_objectTypes[ix1]);
    for (int ix2=0; ix2<tempObjList.size(); ix2++) {
      renderObj = objManagerHandle->getObjectWithID(tempObjList[ix2]);
      if (renderObj) {
          this->addItem(renderObj->getDataObject()->getObjName() + " " + QString::number(tempObjList[ix2]), tempObjList[ix2]);
      }
    }
  }

  // Disconnect previous object
  disconnect(this, SLOT(currObjectModified(int)));

  // Check if the current index is still valid.
  int loc;
  loc = this->findData(m_currentObjectIndex);
  if (loc >= 0) {
    this->setCurrentIndex(loc);

    if (m_currentObjectIndex >= 0) {
      renderObj = objManagerHandle->getObjectWithID(m_currentObjectIndex);
      // Valid object so connect
      if (renderObj) connect(renderObj->getDataObject(), SIGNAL(objectChanged(int)), this, SLOT(currObjectModified(int)));
    }
  } else { 
    m_currentObjectIndex = -1;
    loc = this->findData(m_currentObjectIndex);
    this->setCurrentIndex(loc);
    emit objectSelectionChanged(m_currentObjectIndex);
  }
}
