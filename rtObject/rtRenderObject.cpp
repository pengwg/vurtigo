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
#include "rtRenderObject.h"
#include "rtTimeManager.h"
#include "rtDataObject.h"

rtRenderObject::rtRenderObject() {
  m_dataObj = NULL;
  m_renderName = "None";
  m_objType = rtConstants::OT_None;
  m_treeItem = new QTreeWidgetItem();
  m_treeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  m_visible3D = false;

  // Clear the list first.
  m_pipe3D.clear();
  m_pipe2D.clear();
}

rtRenderObject::~rtRenderObject() {
  if (m_treeItem) delete m_treeItem;
}

//! Get a version of the 3D pipeline that cannot be modified
QList<vtkProp*>* rtRenderObject::get3DPipeline() {
  return &m_pipe3D;
}

//! Get a version of the 2D pipeline that cannot be modified.
QHash<QString, vtkProp*>* rtRenderObject::get2DPipeline() {
  return &m_pipe2D;
}

rtDataObject* rtRenderObject::getDataObject() {
  return m_dataObj;
}

QString rtRenderObject::getName() {
  return m_renderName;
}

rtConstants::rtObjectType rtRenderObject::getObjectType() {
  return m_objType;
}

void rtRenderObject::setDataObject(rtDataObject* dataObj) {
  m_dataObj = dataObj;
}

void rtRenderObject::setName(QString renName) {
  m_renderName = renName;
}

void rtRenderObject::setObjectType(rtConstants::rtObjectType objType) {
  m_objType = objType;
}

//! Try to perform an update for the object
/*!
 This is a thread-safe version of the update() calls.
 @see update()
 @return True if the update was performed.
*/
bool rtRenderObject::tryUpdate() {
  bool res=false;
  if (updateNeeded() && m_dataObj) {
    if(m_dataObj->tryLock()) {
      res = true;
      update();
      resetUpdateTime();
      m_dataObj->unlock();
    }
  }
  return res;
}

void rtRenderObject::updateTreeItem() {
  if (m_treeItem) {
    m_treeItem->setText(0, m_dataObj->getObjName());
    m_treeItem->setText(1, QString::number(m_dataObj->getId()));
    m_treeItem->setTextAlignment(1, Qt::AlignHCenter);

    // Only show the 3D checkbox if there are 3D type of items.
    if (m_pipe3D.count() > 0) {
      m_treeItem->setText(2, "3D");
      if (m_visible3D || m_treeItem->checkState(2) == Qt::Checked) {
        m_treeItem->setCheckState(2,Qt::Checked);
      } else {
        m_treeItem->setCheckState(2,Qt::Unchecked);
      }
    } else {
      m_treeItem->setText(2, "NA");
    }
  }
}

void rtRenderObject::setVisible3D(bool v) {
  m_visible3D = v;
  if (v) {
    tryUpdate();
    rtTimeManager::instance().addToWatchList(this);
  } else {
    rtTimeManager::instance().removeFromWatchList(this);
  }
}

//! Set the update time to the current time.
void rtRenderObject::resetUpdateTime() {
  m_lastUpdate = QDateTime::currentDateTime();
}

//! Check if an update is needed.
bool rtRenderObject::updateNeeded() {
  if(m_dataObj->getModified() > m_lastUpdate) {
    // Modified after the last update.
    return true;
  }
  return false;
}


bool rtRenderObject::saveFile(QFile* file) {
  if (m_dataObj) return m_dataObj->saveFile(file);
  else return false;
}

bool rtRenderObject::loadFile(QFile* file) {
  if (m_dataObj) return m_dataObj->loadFile(file);
  else return false;
}
