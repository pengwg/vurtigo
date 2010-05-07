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
#include "rtObjectManager.h"
#include "rtDataObject.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"
#include "rtMessage.h"
#include "buildParam.h"

// Types of render objects
#include "rt3dPointBufferRenderObject.h"
#include "rtNoneRenderObject.h"
#include "rtLabelRenderObject.h"
#include "rtCathRenderObject.h"
#include "rt3dVolumeRenderObject.h"
#include "rt2dSliceRenderObject.h"
#include "rtMatrixRenderObject.h"
#include "rtPolyRenderObject.h"
#include "rtPieceFuncRenderObject.h"
#include "rtColorFuncRenderObject.h"
#include "rtImageBufferRenderObject.h"
#include "rt2dPointRenderObject.h"
#include "rtEPRenderObject.h"
#include "rtApplication.h"

#include <QMutexLocker>


rtObjectManager::rtObjectManager() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::rtObjectManager() start") );
#endif
  m_max_object = 10000;
  m_objectHash.clear();
  m_list2DHash.clear();
  m_list2DHash.insert(-1, "NONE");
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::rtObjectManager() end") );
#endif
}

//! Object Manager destructor.
rtObjectManager::~rtObjectManager() {
}


//! Create and return a new object.
/*!
  This function will create a new object and add it to the object list. It will then return an instance of that object to the caller. The function returns NULL if the object was not created. Since the Object Manager is the only class that can create objects all other classes must call this particular function to request a new object.
  @param objType The type of object that is to be added
  @param objName The name of the object to be added. All objects have names.
  @return An instance of a new object of a particular type. NULL is returned if the object could not be created.
 */
rtRenderObject* rtObjectManager::addObjectOfType(rtConstants::rtObjectType objType, QString objName) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::addObjectOfType() start") );
#endif

  rtRenderObject* temp = NULL;
  rtDataObject* dataO = NULL;
  int nextID;

  // Lock this function
  QMutexLocker locker(&m_objectLock);

  // Find out which object will be used. 
  switch(objType) {
  case rtConstants::OT_None:
    temp=new rtNoneRenderObject();   
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Warning: None Object Requested. "));
    break;
  case rtConstants::OT_3DObject:
    temp=new rt3DVolumeRenderObject();
    break;
  case rtConstants::OT_2DObject:
    temp=new rt2DSliceRenderObject();
    break;
  case rtConstants::OT_Cath:
    temp=new rtCathRenderObject();
    break;
  case rtConstants::OT_vtkMatrix4x4:
    temp=new rtMatrixRenderObject();
    break;
  case rtConstants::OT_vtkPolyData:
    temp=new rtPolyRenderObject();
    break;
  case rtConstants::OT_vtkPiecewiseFunction:
    temp=new rtPieceFuncRenderObject();
    break;
  case rtConstants::OT_vtkColorTransferFunction:
    temp=new rtColorFuncRenderObject();
    break;
  case rtConstants::OT_ImageBuffer:
    temp=new rtImageBufferRenderObject();
    break;
  case rtConstants::OT_2DPointBuffer:
    temp=new rt2DPointRenderObject();
    break;
  case rtConstants::OT_3DPointBuffer:
    temp=new rt3DPointBufferRenderObject();
    break;
  case rtConstants::OT_TextLabel:
    temp=new rtLabelRenderObject();
    break;
  case rtConstants::OT_EPMesh:
    temp = new rtEPRenderObject();
    break;
  default:
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("No object of type: ").append(QString::number(objType)));
    temp=NULL;
    break;
  }

  // The object has been created.
  if (temp){
    dataO = temp->getDataObject();

    // Try to get the valid ID.
    nextID = dataO->getId();
    if (nextID == -1) {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not find a valid ID for a new object! "));
      delete temp;
      return NULL;
    }

    // Push the new object creation to the log.
    rtApplication::instance().getMessageHandle()->log(QString("Created Object with ID: ").append(QString::number(nextID)));

    dataO->setObjName(objName);
    dataO->update();
    m_objectHash.insert(nextID, temp);
    if ( rtApplication::instance().getMainWinHandle() )
      rtApplication::instance().getMainWinHandle()->updateObjectList(&m_objectHash);

    QList<QString> twoDViews = temp->get2DViewNameList();
    for (int ix1=0; ix1<twoDViews.size(); ix1++) {
      m_list2DHash.insertMulti(nextID, twoDViews[ix1]);
    }
    if (twoDViews.size()>0 && rtApplication::instance().getMainWinHandle()) {
      rtApplication::instance().getMainWinHandle()->update2DWindowLists(&m_list2DHash);
    }

    emit objectCreated(nextID);
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::addObjectOfType() end") );
#endif
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

  // Lock this function
  QMutexLocker locker(&m_objectLock);

  if (m_objectHash.contains(objID)) {
    temp = m_objectHash.value(objID);
    if (!temp || temp->getDataObject()->isReadOnly()) return false;
    m_objectHash.remove(objID);
    if (m_list2DHash.remove(objID)>0 && rtApplication::instance().getMainWinHandle()) {
      rtApplication::instance().getMainWinHandle()->update2DWindowLists(&m_list2DHash);
    }
    delete temp;
    emit objectRemoved(objID);
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

  // Lock this function
  QMutexLocker locker(&m_objectLock);

  if (m_objectHash.contains(objID)) {
    temp = m_objectHash.value(objID);
    if (!temp) return false;
    m_objectHash.remove(objID);
    if (m_list2DHash.remove(objID)>0 && rtApplication::instance().getMainWinHandle()) {
      rtApplication::instance().getMainWinHandle()->update2DWindowLists(&m_list2DHash);
    }
    delete temp;
    emit objectRemoved(objID);
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

int rtObjectManager::getNextID() {
  int i; 

  for (i=1; i<=m_max_object; i++) {
    if ( !m_objectHash.contains(i) ) {
      return i;
    }
  }

  return -1;
}
