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
#include "rtMessage.h"

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
  connect(this, SIGNAL(requestNewObjectSignal(QString,QString)), this, SLOT(requestNewObjectSlot(QString,QString)), Qt::QueuedConnection);
  connect(this,SIGNAL(addNewObjectSignal(QString,rtRenderObjectMaker*)),this,SLOT(addNewObjectSlot(QString,rtRenderObjectMaker*)),Qt::QueuedConnection);

}


int rtBaseHandle::requestNewObject(rtConstants::rtObjectType objType, QString name) {
  int result;

  if (QThread::currentThread() == m_masterThreadPointer) {
    // Master Thread.  No need to use Queued signals and slots.
    // If the lock is busy process some events... This makes waiting more useful.
    while (!m_newObjectLock.tryLock()) QCoreApplication::processEvents();

    rtRenderObject* temp;
    temp = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::objectTypeToQString(objType), name);
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

int rtBaseHandle::requestNewObject(QString objType, QString name) {
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

void rtBaseHandle::addNewObject(QString newType,rtRenderObjectMaker *newObj)
{
    rtConstants::rtObjectType objType;

    if (QThread::currentThread() == m_masterThreadPointer) {
        // Master Thread.  No need to use Queued signals and slots.
        // If the lock is busy process some events... This makes waiting more useful.
        while (!m_newObjectLock.tryLock()) QCoreApplication::processEvents();
        rtApplication::instance().getObjectManager()->addNewObject(newType,newObj);
    }
    else
    {
        // Not in the master thread so we need to pass a message to the master.
        // These threads we lock completely.
        m_newObjectLock.lock();
        // Send the request.
        emit addNewObjectSignal(newType,newObj);
        // Wait for it
        m_newObjectWait.acquire();
    }
    m_newObjectLock.unlock();
}


void rtBaseHandle::requestNewObjectSlot(rtConstants::rtObjectType objType, QString name) {
  rtRenderObject* temp;
  int result=-1;
  temp = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::objectTypeToQString(objType), name);
  if (temp) {
    result = temp->getDataObject()->getId();
  }
  m_newObjectID = result;
  m_newObjectWait.release();
}

void rtBaseHandle::requestNewObjectSlot(QString objType, QString name) {
  rtRenderObject* temp;
  int result=-1;
  temp = rtApplication::instance().getObjectManager()->addObjectOfType(objType, name);
  if (temp) {
    result = temp->getDataObject()->getId();
  }
  m_newObjectID = result;
  m_newObjectWait.release();
}

void rtBaseHandle::addNewObjectSlot(QString newType,rtRenderObjectMaker *newObj)
{
    rtApplication::instance().getObjectManager()->addNewObject(newType,newObj);
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
  return rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::objectTypeToQString(objType));
}

QList<int> rtBaseHandle::getObjectsOfType(QString objType)
{
    return rtApplication::instance().getObjectManager()->getObjectsOfType(objType);
}


int rtBaseHandle::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getNumObjectsOfType(rtConstants::objectTypeToQString(objType));
}

int rtBaseHandle::getNumObjectsOfType(QString objType) {
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

vtkImageData* rtBaseHandle::grabScreenshot(int window) {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow(window);

  vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
  filter->SetInput(temp);
  filter->SetInputBufferTypeToRGB();
  filter->Modified();
  filter->Update();

  m_screen->DeepCopy(filter->GetOutput());

  filter->Delete();
  return m_screen;
}

vtkRenderWindow* rtBaseHandle::getRenderWindow(int window) {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow(window);
  return temp;
}

void rtBaseHandle::forceRenderUpdate(int objID) {
  rtRenderObject* temp=NULL;
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(objID);
  if (temp) {
    temp->tryUpdate();
  }
}

void rtBaseHandle::setObjectVisible3D(int idObj, bool fVisible, int window)
  {
    rtRenderObject *pRenderObj = getRenderObjectWithID(idObj);
    if (!pRenderObj)
      {
        rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtBaseHandle::setObjectVisible3D - idObj  ").append(QString::number(idObj)).append(" does not exist!"));
        return;
      }
      
    rtDataObject   *pDataObj   = pRenderObj->getDataObject();

    if (!pDataObj)
      {
        rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtBaseHandle::setObjectVisible3D - idObj  ").append(QString::number(idObj)).append(" has no data object!"));
        return;
      }
    if ((window > -1) && (window < rtApplication::instance().getMainWinHandle()->getNumRenWins())) {
        pRenderObj->setVisible3D(window,fVisible);
    }
    else
    {
        if (fVisible)
            pRenderObj->setVisible3DAllOn();
        else
            pRenderObj->setVisible3DAllOff();
    }


    pDataObj->Modified();
    pRenderObj->updateTreeItem();
    
  }

void rtBaseHandle::setObjectPermanent(int objID)
{
    rtApplication::instance().getObjectManager()->setPermanent(objID);
}
