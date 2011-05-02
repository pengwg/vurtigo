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
#include "rtApplication.h"
#include "rtMainWindow.h"

rtRenderObject::rtRenderObject()
: m_dataObj(NULL), m_canRender3D(false), m_renderName("None"), m_objType("")
{
  m_treeItem = new QTreeWidgetItem();
  m_treeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

  // Clear the list first.
  m_pipe2D.clear();
  m_syncList.clear();
  m_visible3D.clear();
  for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
      m_visible3D.append(false);
}

rtRenderObject::~rtRenderObject() {
  if (m_treeItem) delete m_treeItem;
}

rtDataObject* rtRenderObject::getDataObject() {
  return m_dataObj;
}

QString rtRenderObject::getName() {
  return m_renderName;
}

QString rtRenderObject::getObjectType()
{
    return m_objType;
}

void rtRenderObject::setDataObject(rtDataObject* dataObj) {
  m_dataObj = dataObj;
}

void rtRenderObject::setName(QString renName) {
  m_renderName = renName;
}

void rtRenderObject::setObjectType(QString objType) {
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

    // Only show the 3D checkbox if the 3D can be rendered.
    if (m_canRender3D) {
      m_treeItem->setText(2, "3D");

      if (m_visible3D.contains(true))
          m_treeItem->setCheckState(2,Qt::Checked);
      else
          m_treeItem->setCheckState(2,Qt::Unchecked);

    } else {
      m_treeItem->setText(2, "NA");
    }
  }
}

void rtRenderObject::uncheckTreeItem() {
  if (m_treeItem) {
      for (int ix1=0; ix1<m_visible3D.size(); ix1++)
          m_visible3D[ix1] = false;
    if (m_treeItem->checkState(2) == Qt::Checked) {
      m_treeItem->setCheckState(2,Qt::Unchecked);
    }
  }
}

void rtRenderObject::setVisible3D(int window,bool v) {
  if( !rtApplication::instance().getTimeManager() ) return;

  if (window >= m_visible3D.size())
      std::cout << "rtRenderObject::setVisible3D tried to set visibilty on a window the object didnt know about \n";
  m_visible3D[window] = v;
  if (v) {
    tryUpdate();
    rtApplication::instance().getTimeManager()->addToWatchList(this);
  } else if (!m_visible3D.contains(true)){
    rtApplication::instance().getTimeManager()->removeFromWatchList(this);
  }
}

void rtRenderObject::setVisible3DAll(QList<bool> v)
{
    if( !rtApplication::instance().getTimeManager() ) return;
    m_visible3D.clear();
    for (int ix1=0; ix1<v.size(); ix1++)
        m_visible3D.append(v[ix1]);
    if (v.contains(true))
    {
        tryUpdate();
        rtApplication::instance().getTimeManager()->addToWatchList(this);
    }
    else
    {
        rtApplication::instance().getTimeManager()->removeFromWatchList(this);
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

void rtRenderObject::removeSyncObject(rtRenderObject *rObj)
{
    for (int ix1=0; ix1<m_syncList.size(); ix1++)
    {
        if (m_syncList.at(ix1) == rObj)
            m_syncList.removeAt(ix1);
    }
}
