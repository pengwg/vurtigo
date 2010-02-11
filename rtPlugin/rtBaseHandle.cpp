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


//! Ask the base to create a new object of a particular type. 
/*!
  Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type.
The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. This is a blocking call as the base needs to allocate objects with a different thread.
Only one thread may request the creation (or removal) of an object at a time.
  @param objType The type of object to be created.
  @param name The name that will be displayed in the object list. 
  @return The ID of the newly created object. Will return -1 if the new object could not be created. 
 */
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

//! This runs in the main execution thread to request an object.
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


//! Remove an object with a specified ID.
/*!
  Request that the base remove an object. Plugins can remove those objects that are not read-only. 
  Only one thread may request to create or remove an object at a time.
  @param ID The ID of the object to be removed. 
  @return True if an object with that ID was removed. False otherwise. 
 */
bool rtBaseHandle::removeObject(int ID) {
  bool res;
  m_newObjectLock.lock();
  res = rtApplication::instance().getObjectManager()->removeObject(ID);
  m_newObjectLock.unlock();
  return res;
}

//! Get a list of IDs for all the objects of a particular type.
/*!
  There may be any number of objects of one type. Given a type of object this function will return a list of IDs corresponding to all of the objects of that type. This function is useful for those plugins that take a specific type of object as input. 
  @see getNumObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return A list of IDs for all the objects of that type. Returns an empty list if there are no objects of that type. 
 */
QList<int> rtBaseHandle::getObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getObjectsOfType(objType);
}

//! Get the number of objects of a particular type. 
/*!
  Get the number of objects of one type. 
  @see getObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return The number of objects of that type
 */
int rtBaseHandle::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getNumObjectsOfType(objType);
}

//! Get the object that has this ID
/*!
  Each object in the base has a unique ID. This funciton requests a pointer to the object that has a certain ID. The object may be modified but the pointer cannot be changed. 
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists or the reqested object is Read-Only.
 */
rtDataObject* const rtBaseHandle::getObjectWithID(int ID) {
  rtRenderObject* temp=NULL;

  temp = rtApplication::instance().getObjectManager()->getObjectWithID(ID);
  if (temp && !temp->getDataObject()->isReadOnly()) {
    return temp->getDataObject();
  }

  return NULL;
}

//! Get the read-only object that has this ID
/*!
  Each object in the base has a unique ID. Some objects in the base are considered read-only and the plugins may only access them via this function. This funciton requests a pointer to the object that has a certain ID. The object may be NOT be modified and the pointer cannot be changed. 
  @see getObjectWithID()
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists.
 */
const rtDataObject* const rtBaseHandle::getROObjectWithID(int ID) {
  rtRenderObject* temp=NULL;

  temp = rtApplication::instance().getObjectManager()->getObjectWithID(ID);
  if (temp) {
    return temp->getDataObject();
  }
  return NULL;
}

//! Add a watch to clicks on the main 3D window.
/*!
  The plugin can register with the base to receive a callback every time the user sets a point in the 3D view. 
  \param pluginID The ID of the plugin to register.
  \param watch True if the watch is to be added. False to remove a watch.
  \return true if the watch was added/removed as was requested by the caller.
 */
bool rtBaseHandle::watchClick(int pluginID, bool watch) {
  bool res;

  if (watch) {
    res = rtApplication::instance().getPluginLoader()->addToClickWatch(pluginID);
  } else {
    res = rtApplication::instance().getPluginLoader()->removeFromClickWatch(pluginID);
  }
  return res;
}

//! Request a new screenshot. NOT THREAD SAFE!
/*!
  This function needs to access the graphics directly so it has to be executed in the same thread as the rendering pipeline. NOT THREAD SAFE!
  */
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


  //! Get a pointer to the Vurtigo Render Window. NOT THREAD SAFE!
/*!
  This function returns a pointer to a modifyable render window. NOT THREAD SAFE!
The user should be careful when using this pointer as changes to the render window object may have unexpected results.
The use of the vtkRenderWindow object in a thread other than the main Vurtigo rendering thread is not cosidered safe.
  */
vtkRenderWindow* rtBaseHandle::getRenderWindow() {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow();
  return temp;
}

//! Force the GUI to do a render update.
/*!
  Renderable objects will not get updated by the base unless they are being rendered.
  This is desired behaviour since it is usually wasteful to update an object that is not being rendered. However in some cases it may be needed to force an update.
  Plugin developers should use caution when calling this function since it may cause Vurtigo to slow down if abused.
  @param objID The ID of the object to be updated.
  */
void rtBaseHandle::forceRenderUpdate(int objID) {
  rtRenderObject* temp=NULL;
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(objID);
  if (temp) {
    temp->tryUpdate();
  }
}
