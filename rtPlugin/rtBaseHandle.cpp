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

/*!
  Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type.
The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. This is a blocking call as the base needs to allocate objects with a different thread.
Only one thread may request the creation (or removal) of an object at a time.
  @param objType The type of object to be created (using old enumerated type).
  @param name The name that will be displayed in the object list.
  @return The ID of the newly created object. Will return -1 if the new object could not be created.
  @deprecated Use requestNewObject(QString.QString)
 */
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

/*!
  Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type.
The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. This is a blocking call as the base needs to allocate objects with a different thread.
Only one thread may request the creation (or removal) of an object at a time.
  @param objType The type of object to be created as a QString.
  @param name The name that will be displayed in the object list.
  @return The ID of the newly created object. Will return -1 if the new object could not be created.
 */
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

/*!
  This function is a request from the plugin to the data manager for the addition of a new object type.
The data manager will add the object type to Vurtigo so subsequent calls to requestNewObject can include the new type and Vurtigo can invoke the given
Maker object to create a new object.
  @param newType The new type
  @param newObj An object that can make that type of object
 */
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

/*!
  This function is a request from the plugin to the main window handler to connect the given object
  to the given render window widget in order to process QEvents (mouse/keyboard) that are generated.
  Used in rtRenderObject::addToRenderer() method
  @param obj The render object we want to connect
  @param window The id of the render window widget we want to connect to
  */
void rtBaseHandle::connectObjectToEvents(rtRenderObject *obj, int window)
{
    // Connect signals and slots for interaction.
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);
    // Connect mouse actions
    connect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), obj, SLOT(mousePressEvent(QMouseEvent*,int)));
    connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), obj, SLOT(mouseMoveEvent(QMouseEvent*,int)));
    connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), obj, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
    connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), obj, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
    connect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), obj, SLOT(keyPressEvent(QKeyEvent*,int)));
    connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), obj, SLOT(keyReleaseEvent(QKeyEvent*,int)));
    connect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), obj, SLOT(wheelEvent(QWheelEvent*,int)));
}

/*!
  This function is a request from the plugin to the main window handler to disconnect the given object
  from the given render window widget in order to stop processing QEvents (mouse/keyboard) that are generated.
  Used in rtRenderObject::removeFomRenderer() method
  @param obj The render object we want to disconnect
  @param window The id of the render window widget we want to disconnect from
  */
void rtBaseHandle::disconnectObjectFromEvents(rtRenderObject *obj, int window)
{
    customQVTKWidget* renWid;
    renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);

    // Disconnect mouse actions
    disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), obj, SLOT(mousePressEvent(QMouseEvent*,int)));
    disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), obj, SLOT(mouseMoveEvent(QMouseEvent*,int)));
    disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), obj, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
    disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), obj, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
    disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), obj, SLOT(keyPressEvent(QKeyEvent*,int)));
    disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), obj, SLOT(keyReleaseEvent(QKeyEvent*,int)));
    disconnect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), obj, SLOT(wheelEvent(QWheelEvent*,int)));
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

/*!
  There may be any number of objects of one type. Given a type of object this function will return a list of IDs corresponding to all of the objects of that type. This function is useful for those plugins that take a specific type of object as input.
  @see getNumObjectsOfType()
  @param objType The type of object that the plugin is looking for (using old enumerated type).
  @return A list of IDs for all the objects of that type. Returns an empty list if there are no objects of that type.
  @deprecated Use getObjectsOfType(QString)
 */
QList<int> rtBaseHandle::getObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getObjectsOfType(rtConstants::objectTypeToQString(objType));
}

/*!
  There may be any number of objects of one type. Given a type of object this function will return a list of IDs corresponding to all of the objects of that type. This function is useful for those plugins that take a specific type of object as input.
  @see getNumObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return A list of IDs for all the objects of that type. Returns an empty list if there are no objects of that type.
 */
QList<int> rtBaseHandle::getObjectsOfType(QString objType)
{
    return rtApplication::instance().getObjectManager()->getObjectsOfType(objType);
}

/*!
  Get the number of objects of one type.
  @see getObjectsOfType()
  @param objType The type of object that the plugin is looking for (Using old enumerated type).
  @return The number of objects of that type
  @deprecated Use getNumObjectsOfType(QString)
 */
int rtBaseHandle::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  return rtApplication::instance().getObjectManager()->getNumObjectsOfType(rtConstants::objectTypeToQString(objType));
}

/*!
  Get the number of objects of one type.
  @see getObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return The number of objects of that type
 */
int rtBaseHandle::getNumObjectsOfType(QString objType) {
  return rtApplication::instance().getObjectManager()->getNumObjectsOfType(objType);
}

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

/*!
Used internally to request a render object for changing its rendering properties
@param ID The id number of the requested object.
@return A pointer to the object or NULL if no object with this ID exists or the reqested object is Read-Only.
*/
rtRenderObject* const rtBaseHandle::getRenderObjectWithID(int ID) {
  return rtApplication::instance().getObjectManager()->getObjectWithID(ID);
}

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

/*!
  This function needs to access the graphics directly so it has to be executed in the same thread as the rendering pipeline. NOT THREAD SAFE!
  */
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

/*!
  This function returns a pointer to a modifyable render window. NOT THREAD SAFE!
The user should be careful when using this pointer as changes to the render window object may have unexpected results.
The use of the vtkRenderWindow object in a thread other than the main Vurtigo rendering thread is not cosidered safe.
  */
vtkRenderWindow* rtBaseHandle::getRenderWindow(int window) {
  vtkRenderWindow* temp;

  temp = rtApplication::instance().getMainWinHandle()->getRenderWindow(window);
  return temp;
}

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

/*!
  This will also check/uncheck the 3D box for the given object in the Object Browser. It will only have an effect if the object can be rendered in 3D.
  @param idObj The ID of the object
  @param fVisible true to view the object in 3d, false otherwise
  @param window the render window you want to change visiblity in. If -1, will apply to all windows
  */
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

QList<bool> rtBaseHandle::getObjectVisible3D(int objID)
{
    rtRenderObject *rObj = getRenderObjectWithID(objID);
    return rObj->getVisible3D();
}

void rtBaseHandle::setObjectPermanent(int objID)
{
    rtApplication::instance().getObjectManager()->setPermanent(objID);
}
