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

  //! Ask the base to create a new object of a particular type.
/*!
  Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type.
The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. This is a blocking call as the base needs to allocate objects with a different thread.
Only one thread may request the creation (or removal) of an object at a time.
  @param objType The type of object to be created.
  @param name The name that will be displayed in the object list.
  @return The ID of the newly created object. Will return -1 if the new object could not be created.
 */
  int requestNewObject(rtConstants::rtObjectType objType, QString name="pluginObj");

  //! Ask the base to create a new object of a particular type.
 /*!
   Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type.
 The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. This is a blocking call as the base needs to allocate objects with a different thread.
 Only one thread may request the creation (or removal) of an object at a time.
   @param objType The type of object to be created as a QString.
   @param name The name that will be displayed in the object list.
   @return The ID of the newly created object. Will return -1 if the new object could not be created.
  */
   int requestNewObject(QString objType, QString name="pluginObj");


   //! Ask the base to add a given object.
 /*!
   This function is a request from the plugin to the data manager for the addition of a new object type.
 The data manager will add the object type to Vurtigo so subsequent calls to requestNewObject can include the new type and Vurtigo can invoke the given
 Maker object to create a new object.
   @param newObj An object that can make that type of object
  */
   void addNewObject(QString newType,rtRenderObjectMaker *newObj);


//! Remove an object with a specified ID.
/*!
  Request that the base remove an object. Plugins can remove those objects that are not read-only.
  Only one thread may request to create or remove an object at a time.
  @param ID The ID of the object to be removed.
  @return True if an object with that ID was removed. False otherwise.
 */
  bool removeObject(int ID);

//! Get a list of IDs for all the objects of a particular type.
/*!
  There may be any number of objects of one type. Given a type of object this function will return a list of IDs corresponding to all of the objects of that type. This function is useful for those plugins that take a specific type of object as input.
  @see getNumObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return A list of IDs for all the objects of that type. Returns an empty list if there are no objects of that type.
 */
  QList<int> getObjectsOfType(rtConstants::rtObjectType objType);

//! Get the number of objects of a particular type.
/*!
  Get the number of objects of one type.
  @see getObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return The number of objects of that type
 */
  int getNumObjectsOfType(rtConstants::rtObjectType objType);

  //! Get the object that has this ID
  /*!
  Each object in the base has a unique ID. This funciton requests a pointer to the object that has a certain ID. The object may be modified but the pointer cannot be changed.
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists or the reqested object is Read-Only.
  */
  rtDataObject* const getObjectWithID(int ID);

  //! Get the read-only object that has this ID
/*!
  Each object in the base has a unique ID. Some objects in the base are considered read-only and the plugins may only access them via this function. This funciton requests a pointer to the object that has a certain ID. The object may be NOT be modified and the pointer cannot be changed.
  @see getObjectWithID()
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists.
 */
  const rtDataObject* const getROObjectWithID(int ID);


  //! Add a watch to clicks on the main 3D window.
/*!
  The plugin can register with the base to receive a callback every time the user sets a point in the 3D view.
  \param pluginID The ID of the plugin to register.
  \param watch True if the watch is to be added. False to remove a watch.
  \return true if the watch was added/removed as was requested by the caller.
 */
  bool watchClick(int pluginID, bool watch);

  //! Request a new screenshot. NOT THREAD SAFE!
/*!
  This function needs to access the graphics directly so it has to be executed in the same thread as the rendering pipeline. NOT THREAD SAFE!
  */
  vtkImageData* grabScreenshot(int window = 0);

  //! Get a pointer to one of the Vurtigo Render Windows. NOT THREAD SAFE!
/*!
  This function returns a pointer to a modifyable render window. NOT THREAD SAFE!
The user should be careful when using this pointer as changes to the render window object may have unexpected results.
The use of the vtkRenderWindow object in a thread other than the main Vurtigo rendering thread is not cosidered safe.
  */
  vtkRenderWindow* getRenderWindow(int window = 0);

//! Force the GUI to do a render update.
/*!
  Renderable objects will not get updated by the base unless they are being rendered.
  This is desired behaviour since it is usually wasteful to update an object that is not being rendered. However in some cases it may be needed to force an update.
  Plugin developers should use caution when calling this function since it may cause Vurtigo to slow down if abused.
  @param objID The ID of the object to be updated.
  */
  void forceRenderUpdate(int objID);

  //! Toggle the visibility of an object in a given 3d view
  /*!
    This will also check/uncheck the 3D box for the given object in the Object Browser. It will only have an effect if the object can be rendered in 3D.
    @param idObj The ID of the object
    @param fVisible true to view the object in 3d, false otherwise
    @param window the render window you want to change visiblity in. If -1, will apply to all windows
    */
  void setObjectVisible3D(int idObj, bool fVisible, int window = -1);

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
  /*!
  Used internally to request a render object for changing its rendering properties
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists or the reqested object is Read-Only.
  */
  rtRenderObject* const getRenderObjectWithID(int ID);
  


};

#endif
