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
#ifndef RT_BASE_HANDLE_H
#define RT_BASE_HANDLE_H

#include "objTypes.h"
#include <QList>
#include <QString>
#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QThread>

class rtDataObject;
class rtRenderObject;
class rtRenderObjectMaker;

#include "vtkRenderWindow.h"
#include "vtkImageData.h"

//! The handle that the Plugin can use to interact with Vurtigo. [Singleton]
/*!
  Plugins may need to ask the base for instances of object or they may need to register for callbacks. This base handle is the way plugins should access the base. The functions here are specifically designed for the plugins to use.
  It is higly reccomnded that the plugins SHOULD NOT use the other singleton classes directly. There are plugin-specific safety features set in the rtBaseHandle class that protect the functioning of the base. If the other classes are used directly then these features are bypassed and as a result the plugin can cause bugs in the base.
  This class is also a singleton meaning that there will only be one instance of it for the whole program. Plugins should get access to this via the instance() function.
  Functions in this class are usually thread safe. Those functions that are not thread safe are specified in the docs for that function.
 */
class rtBaseHandle : public QObject {

 Q_OBJECT

 public:
  //! Destructor
  ~rtBaseHandle();

  //! Get the instance of rtBaseHandle
  static rtBaseHandle& instance();

  //! Ask the base to create a new object of a particular type
  int requestNewObject(rtConstants::rtObjectType objType, QString name="pluginObj");

  //! Ask the base to create a new object of a particular type.
  int requestNewObject(QString objType, QString name="pluginObj");


  //! Ask the base to add a new object type with its object maker.
  void addNewObject(QString newType,rtRenderObjectMaker *newObj);

  //! Ask the base to connect this object to QEvents
  void connectObjectToEvents(rtRenderObject *obj, int window);

  //! Ask the base to disconnect this object from QEvents
  void disconnectObjectFromEvents(rtRenderObject *obj, int window);


  //! Remove an object with a specified ID.
  bool removeObject(int ID);

  //! Get a list of IDs for all the objects of a particular type.
  QList<int> getObjectsOfType(rtConstants::rtObjectType objType);

  //! Get a list of IDs for all the objects of a particular type.
  QList<int> getObjectsOfType(QString objType);

  //! Get the number of objects of a particular type.
  int getNumObjectsOfType(rtConstants::rtObjectType objType);

  //! Get the number of objects of a particular type.
  int getNumObjectsOfType(QString objType);

  //! Get the object that has this ID
  rtDataObject* const getObjectWithID(int ID);

  //! Get the read-only object that has this ID
  const rtDataObject* const getROObjectWithID(int ID);


  //! Add a watch to clicks on the main 3D window.
  bool watchClick(int pluginID, bool watch);

  //! Request a new screenshot. NOT THREAD SAFE!
  vtkImageData* grabScreenshot(int window = 0);

  //! Get a pointer to one of the Vurtigo Render Windows. NOT THREAD SAFE!
  vtkRenderWindow* getRenderWindow(int window = 0);

  //! Force the GUI to do a render update.
  void forceRenderUpdate(int objID);

  //! Toggle the visibility of an object in a given 3d view
  void setObjectVisible3D(int idObj, bool fVisible, int window = -1);

  //! Get the visibilities of an object
  QList<bool> getObjectVisible3D(int objID);

  //! Set an object to be permanent (cannot be deleted)
  void setObjectPermanent(int objID);

 public slots:
  //! This runs in the main execution thread to request an object.
  void requestNewObjectSlot(rtConstants::rtObjectType, QString);
  //! This runs in the main execution thread to request an object.
  void requestNewObjectSlot(QString, QString);
  //! This run in he main execution thread to add an object.
  void addNewObjectSlot(QString,rtRenderObjectMaker *);
 signals:
  //! Signal sent when an object is requested
  void requestNewObjectSignal(rtConstants::rtObjectType, QString);
  //! Signal sent when an object is requested
  void requestNewObjectSignal(QString, QString);
  //! Signal sent when an object is added
  void addNewObjectSignal(QString,rtRenderObjectMaker *);

 protected:
  //! Connect the signals to the slots for the base handle.
  void connectSignals();

  //! The id of the new object being requested.
  int m_newObjectID;
  QMutex m_newObjectLock;
  QSemaphore m_newObjectWait;

  //! A pointer to the main Vurtigo thread.
  /*!
    This pointer enables the base handle to determine if it is currently running in the main thread.
    */
  QThread* m_masterThreadPointer;

  //! Temporary storaage location for a screenshot if it is requested by the plug-in.
  vtkImageData* m_screen;

 private:
  //! Private constructor
  rtBaseHandle();
  //! Private copy-constructor
  rtBaseHandle(const rtBaseHandle&);
  //! Private equals operator.
  rtBaseHandle& operator=(const rtBaseHandle&);
  //! Get the render object corresponding with the data object that has this ID
  rtRenderObject* const getRenderObjectWithID(int ID);
  


};

#endif
