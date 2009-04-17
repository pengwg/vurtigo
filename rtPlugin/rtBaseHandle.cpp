#include "rtBaseHandle.h"
#include "rtDataObject.h"

rtBaseHandle::rtBaseHandle() {

}

//! Destructor
rtBaseHandle::~rtBaseHandle() {

}

//! Ask the base to create a new object of a particular type. 
/*!
  Plugins cannot create objects or datasets directly. Only the object manager can create objects. This function is a request from the plugin to the data manager for the creation of a new object of a particular type. The data manager will create the object and then return the ID to the plugin. The plugin can then modify the object with that ID. 
  @param objType The type of object to be created.
  @return The ID of the newly created object. Will return -1 if the new object could not be created. 
 */
int rtBaseHandle::requestNewObject(rtConstants::rtObjectType objType) {
  return -1;
}

//! Remove an object with a specified ID.
/*!
  Request that the base remove an object. Plugins can remove those objects that are not read-only. 
  @param ID The ID of the object to be removed. 
  @return True if an object with that ID was removed. False otherwise. 
 */
bool rtBaseHandle::removeObject(int ID) {
  return false;
}

//! Get a list of IDs for all the objects of a particular type.
/*!
  There may be any number of objects of one type. Given a type of object this function will return a list of IDs corresponding to all of the objects of that type. This function is useful for those plugins that take a specific type of object as input. 
  @see getNumObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return A list of IDs for all the objects of that type. Returns an empty list if there are no objects of that type. 
 */
QList<int> rtBaseHandle::getObjectsOfType(rtConstants::rtObjectType objType) {

}

//! Get the number of objects of a particular type. 
/*!
  Get the number of objects of one type. 
  @see getObjectsOfType()
  @param objType The type of object that the plugin is looking for.
  @return The number of objects of that type
 */
int rtBaseHandle::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  return 0;
}

//! Get the object that has this ID
/*!
  Each object in the base has a unique ID. This funciton requests a pointer to the object that has a certain ID. The object may be modified but the pointer cannot be changed. 
  @param ID The id number of the requested object.
  @return A pointer to the object or NULL if no object with this ID exists.
 */
rtDataObject* const rtBaseHandle::getObjectWithID(int ID) {
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
  return NULL;
}

//! Add or remove a watch from an object. 
/*!
  If a watch is added to an object that is already being watched or removed from an object that is not being watched this function has no effect. 

  By default plugins will not be notified if an object changes. To obtain notification a plugin can call this function. A plugin may call this function multiple times with different object IDs to obtain notifications on changes from multiple objects. However, it is reccomended that plugins do not register for all possible data sets. This will slow the base. 
  @param ID The id number of the object to watch. 
  @param watch Set this to true to watch an object or false to remove the watch.
  @return True if the watch was set as desired. 
 */
bool rtBaseHandle::watchObject(int ID, bool watch) {
  return false;
}

//! Add a watch to clicks on the main 3D window.
/*!
  The plugin can register with the base to receive a callback every time the user sets a point in the 3D view. 
  @param watch True if the watch is to be added. False to remove a watch.
  @return true if the watch was added/removed as was requested by the caller.
 */
bool rtBaseHandle::watchClick(bool watch) {
  return false;
}

//! Add a timer to call update at regular intervals
/*!
  The plugin will register with the base to make periodic update calls. The base will try to make the calls with correct timing however this is not guaranteed. The base will put at least this much time between calls to update but the delay time may be longer based on the load of the base and the other plugins. Each plugin may only have one update timer.
  @param milis Number of miliseconds between calls to update. The minimum value is 10 for this parameter. Lower values will default to 10.
  @param watch Set to true to add the timer and flase to remove it.
  @return true if the timer was added/removed as requested.
 */
bool rtBaseHandle::timerUpdate(int milis, bool watch) {
  int inter;

  if (milis < 10) { 
    inter = 10;
  } else {
    inter = milis;
  }

  return false;
  
}
