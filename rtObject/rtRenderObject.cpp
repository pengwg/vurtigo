/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
  if (m_dataObj) delete m_dataObj;
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
  // This is to take care of the case when removeFromRenderer is called on an object deletion rather than just turning off visibility
  // In the case of deletion, the actor must be removed from every render window (even those not currently visible).
  // Rather than adding this check to every removeFromRenderer method in every object, I put the check here.
  if (window >= m_visible3D.size()) return;
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

void rtRenderObject::saveVisibilities(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Visibilities");
    for (int ix1=0; ix1<this->getVisible3D().size(); ix1++)
    {
        writer->writeAttribute(QString("vis" + QString::number(ix1)),QString::number(this->getVisible3D().at(ix1)));
    }
    writer->writeEndElement();
}

void rtRenderObject::loadVisibilities(QXmlStreamReader *reader)
{
    if (!reader) return;

    QXmlStreamAttributes attribs = reader->attributes();

    while (rtApplication::instance().getMainWinHandle()->getNumRenWins() < attribs.size())
        rtApplication::instance().getMainWinHandle()->addRenWinPressed();

    while (rtApplication::instance().getMainWinHandle()->getNumRenWins() > attribs.size())
        rtApplication::instance().getMainWinHandle()->remRenWinPressed();

    QList<bool> vis;
    for (int ix1=0; ix1<attribs.size(); ix1++)
    {
        vis.append(attribs.value(QString("vis"+QString::number(ix1))).toString().toInt());
    }
    this->setVisible3DAll(vis);

}
