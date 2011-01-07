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
#include "rtBaseHandle.h"
#include "rtDataObject.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "rtPluginLoader.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

#include <QCoreApplication>


#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkImageData.h>

rtBaseHandle::rtBaseHandle() {
  connectSignals();

  // A newly requested object has its ID placed here.
  m_newObjectID = -1;

  m_masterThreadPointer = QThread::currentThread();
  m_screen = vtkImageData::New();
}

rtBaseHandle::~rtBaseHandle() {
  m_screen->Delete();
}

rtBaseHandle& rtBaseHandle::instance() {
  static rtBaseHandle handle;
  return handle;
}

//! Connect the signals to the slots.
void rtBaseHandle::connectSignals() {
  qRegisterMetaType<rtConstants::rtObjectType>("rtConstants::rtObjectType");

  connect(this, SIGNAL(requestNewObjectSignal(rtConstants::rtObjectType,QString)), this, SLOT(requestNewObjectSlot(rtConstants::rtObjectType,QString)), Qt::QueuedConnection);


}


int rtBaseHandle::requestNewObject(rtConstants::rtObjectType objType, QString name) {
  int result;

  if (QThread::currentThread() == m_masterThreadPointer) {
    // Master Thread.  No need to use Queued signals and slots.
    // If the lock is busy process some events... This makes waiting more useful.
    while (!m_newObjectLock.tryLock()) QCoreApplication::processEvents();

    rtRenderObject* temp;
    temp = rtApplication::instance().getObjectManager()->addObjectOfType(objType, name);
    if (temp) {
      result = temp->getDataObject()->getId();
    }
  } else {
    // Not in the master thread so we need to pass a message to the master.
    // These threads we lock completely.
    m_newObjectLock.lock();
    // Send the request.
    emit requestNewObjectSignal(objType, name);
    // Wait for it
    m_newObjectWait.acquire();
    // Copy the result
    result = m_newObjectID;
  }
  m_newObjectLock.unlock();
  return result;
}


void rtBaseHandle::requestNewObjectSlot(rtConstants::rtObjectType objType, QString name) {
  rtRenderObject* temp;
  int result=-1;
  temp = rtApplication::instance().getObjectManager()->addObjectOfType(objType, name);
  if (temp) {
    result = temp->getDataObject()->getId();
  }
  m_newObjectID = result;
  m_newObjectWait.release();
}


bool rtBaseHandle::removeObject(int ID) {
  bool res;
  m_newObjectLock.lock();
  res = rtApplication::instance().getObjectManager()->removeObject(ID);
  m_newObjectLock.unlock();
  return res;
}

QList<int> rtBaseHandle::getObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getObjectsOfType(objType);
}


int rtBaseHandle::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getNumObjectsOfType(objType);
}

rtDataObject* const rtBaseHandle::getObjectWithID(int ID) {
  rtRenderObject* temp=NULL;

  temp = rtApplication::instance().getObjectManager()->getObjectWithID(ID);
  if (temp && !temp->getDataObject()->isReadOnly()) {
    return temp->getDataObject();
  }

  return NULL;
}

const rtDataObject* const rtBaseHandle::getROObjectWithID(int ID) {
  rtRenderObject* temp=NULL;

  temp = rtApplication::instance().getObjectManager()->getObjectWithID(ID);
  if (temp) {
    return temp->getDataObject();
  }
  return NULL;
}

rtRenderObject* const rtBaseHandle::getRenderObjectWithID(int ID) {
  return rtApplication::instance().getObjectManager()->getObjectWithID(ID);
}


bool rtBaseHandle::watchClick(int pluginID, bool watch) {
  bool res;

  if (watch) {
    res = rtApplication::instance().getPluginLoader()->addToClickWatch(pluginID);
  } else {
    res = rtApplication::instance().getPluginLoader()->removeFromClickWatch(pluginID);
  }
  return res;
}

vtkImageData* rtBaseHandle::grabScreenshot() {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow();

  vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
  filter->SetInput(temp);
  filter->SetInputBufferTypeToRGB();
  filter->Modified();
  filter->Update();

  m_screen->DeepCopy(filter->GetOutput());

  filter->Delete();
  return m_screen;
}

vtkRenderWindow* rtBaseHandle::getRenderWindow() {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow();
  return temp;
}

void rtBaseHandle::forceRenderUpdate(int objID) {
  rtRenderObject* temp=NULL;
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(objID);
  if (temp) {
    temp->tryUpdate();
  }
}

void rtBaseHandle::setObjectVisible3D(int idObj, bool fVisible)
  {
    rtRenderObject *pRenderObj = getRenderObjectWithID(idObj);
    if (!pRenderObj)
      {
        cerr << "ERROR: rtBaseHandle::setObjectVisible3D - idObj " << idObj << " does not exist!" << endl;
        return;
      }
      
    rtDataObject   *pDataObj   = pRenderObj->getDataObject();

    if (!pDataObj)
      {
        cerr << "ERROR: rtBaseHandle::setObjectVisible3D - idObj " << idObj << " has no data object!" << endl;
        return;
      }
    
    pRenderObj->setVisible3D(fVisible);
    pDataObj->Modified();
    pRenderObj->updateTreeItem();
    
  }
