#include "rtObjectManager.h"
#include "rtDataObject.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"

// Types of render objects
#include "rt3dPointBufferRenderObject.h"
#include "rtNoneRenderObject.h"
#include "rtLabelRenderObject.h"
#include "rtCathRenderObject.h"
#include "rt3dVolumeRenderObject.h"

//! Object Manager constructor.
/*!
  Among other things, the constructor sets the maximum number of objects.
 */
rtObjectManager::rtObjectManager() {
  m_max_object = 10000;
  m_objectHash.clear();
  m_mainWinHandle = NULL;
}

//! Object Manager destructor.
rtObjectManager::~rtObjectManager() {
}

//! Give the object manager the main window object.
/*!
  The object manager needs to communicate with the GUI so it needs to be given an instance of the handle.  
*/
void rtObjectManager::setMainWinHandle(rtMainWindow* mainWin) {
  m_mainWinHandle = mainWin;
}


//! Return an instance of the main window handle.
/*!
  Objects that make use of the object manager can get a handle to the main window through the manager. 
  @return Instance of the main window handle or NULL if no such handle exists. 
*/
rtMainWindow* rtObjectManager::getMainWinHandle() {
  return m_mainWinHandle;
}


//! Create and return a new object.
/*!
 This function will create a new object and add it to the object list. It will then return an instance of that object to the caller. The function returns NULL if the object was not created. Since the Object Manager is the only class that can create objects all other classes must call this particular function to request a new object. 
 @return An instance of a new object of a particular type. NULL is returned if the object could not be created.
 */
rtRenderObject* rtObjectManager::addObjectOfType(rtConstants::rtObjectType objType, QString objName) {
  rtRenderObject* temp = NULL;
  rtDataObject* dataO = NULL;
  int nextID;

  // Try to get a valid ID. 
  nextID = getNextID();
  if (nextID == -1) return NULL;
 
  // Find out which object will be used. 
  switch(objType) {
  case rtConstants::OT_None:
    temp=new rtNoneRenderObject();
    qWarning("Warning: None Object Requested.");
    break;
  case rtConstants::OT_4DObject:
    temp=NULL;
    break;
  case rtConstants::OT_3DObject:
    temp=new rt3DVolumeRenderObject();
    break;
  case rtConstants::OT_2DObject:
    temp=NULL;
    break;
  case rtConstants::OT_Cath:
    temp=new rtCathRenderObject();
    break;
  case rtConstants::OT_vtkMatrix4x4:
    temp=NULL;
    break;
  case rtConstants::OT_vtkPolyData:
    temp=NULL;
    break;
  case rtConstants::OT_vtkPiecewiseFunction:
    temp=NULL;
    break;
  case rtConstants::OT_vtkColorTransferFunction:
    temp=NULL;
    break;
  case rtConstants::OT_ImageBuffer:
    temp=NULL;
    break;
  case rtConstants::OT_2DPointBuffer:
    temp=NULL;
    break;
  case rtConstants::OT_3DPointBuffer:
    temp=new rt3DPointBufferRenderObject();
    break;
  case rtConstants::OT_TextLabel:
    temp=new rtLabelRenderObject();
    break;
  default:
    temp=NULL;
    break;
  }

  // Debug Statement:
  qDebug("Created Object with ID: %d", nextID);

  // The object has been created.
  if (temp){
    dataO = temp->getDataObject();
    dataO->setId(nextID);
    dataO->setObjName(objName);
    dataO->update();
    m_objectHash.insert(nextID, temp);
    if (m_mainWinHandle) m_mainWinHandle->updateObjectList(&m_objectHash);
    temp->setMainWindow(m_mainWinHandle);
  }
  
  return temp;
}

//! Create and return a new read-only object.
/*!
  Create a new read-only object of a particular type. This function should only be called by the base and not by any plugin. To create a read-only object the base will first create a R/W object and then set the Read-Only flag. 
  @see addObjectOfType()
  @return An instance of a new object of a particular type. NULL is returned if the object could not be created. 
 */
rtRenderObject* rtObjectManager::addReadOnlyObject(rtConstants::rtObjectType objType, QString objName) {
  rtRenderObject* newObj;

  newObj = addObjectOfType(objType, objName);
  if (newObj) newObj->getDataObject()->setReadOnly();
  return newObj;
}

//! Remove and delete an object from the list.
/*!
  Can only remove read/write objects. For read only objects see removeReadOnly().
  @see removeReadOnly()
  @return True if the object was deleted as requested. False otherwise.
 */
bool rtObjectManager::removeObject(int objID) {
  rtRenderObject* temp;

  if (m_objectHash.contains(objID)) {
    temp = m_objectHash.value(objID);
    if (!temp || temp->getDataObject()->isReadOnly()) return false;
    m_objectHash.remove(objID);
    delete temp;
    return true;
  }
  return false;
}

//! The only way to remove read-only objects
/*!
  Remove a read-only object. Only the base should  make use of this function. 
  @see removeObject()
  @return True if the object was deleted as requested. False otherwise.
 */
bool rtObjectManager::removeReadOnly(int objID) {
  rtRenderObject* temp;

  if (m_objectHash.contains(objID)) {
    temp = m_objectHash.value(objID);
    if (!temp) return false;
    m_objectHash.remove(objID);
    delete temp;
    return true;
  }
  return false;
}

//! Get the object with a particular ID.
/*!
  Get an object that exists and has the specified ID. The object will not be created if it does not exist.
  @see addObjectOfType()
  @return The object with that ID or NULL if no such object exists. 
 */
rtRenderObject* rtObjectManager::getObjectWithID(int objID) {
  return m_objectHash.value(objID);
}

//! Get a list of IDs for all objects of a particular type. 
/*!
  A QList of IDs will be returned to the caller. This operation is expensive if there are many objects of that type since the QList is copied over to the caller. A long list will take a long time to traverse and copy. 
  @return A list of IDs. If there are no objects of that type the list will be empty.
 */
QList<int> rtObjectManager::getObjectsOfType(rtConstants::rtObjectType objType) {
  QList<int> objList;
  
  // Start with an empty list.
  objList.clear();

  QHashIterator<int, rtRenderObject*> i(m_objectHash);
  while (i.hasNext()) {
    i.next();
    if (i.value()->getDataObject()->getObjectType() == objType) {
      objList.append(i.key());
    }
  }

  return objList;
}

//! Get the number of objects of one type. 
/*!
  @return The number of objects of that type.
 */
int rtObjectManager::getNumObjectsOfType(rtConstants::rtObjectType objType) {
  int numObj = 0;

  QHashIterator<int, rtRenderObject*> i(m_objectHash);
  while (i.hasNext()) {
    i.next();
    if (i.value()->getDataObject()->getObjectType() == objType) {
      numObj++;
    }
  }

  return numObj;
}

//! Get the next unused ID. 
/*!
  @return The next unused ID
 */
int rtObjectManager::getNextID() {
  int i; 

  for (i=1; i<=m_max_object; i++) {
    if ( !m_objectHash.contains(i) ) {
      return i;
    }
  }

  return -1;
}
