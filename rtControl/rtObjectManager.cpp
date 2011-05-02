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
#include "rtRenderObjectMaker.h"
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
  m_extObjects.clear();
  m_list2DHash.insert(-1, "NONE");
  for (int i=0; i<12; i++)
  {
      m_objectTypeList.append(rtConstants::objectTypeToQString(rtConstants::intToObjectType(i)));
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::rtObjectManager() end") );
#endif
}

//! Object Manager destructor.
rtObjectManager::~rtObjectManager() {
    for (int ix1=0; ix1<m_extObjects.size(); ix1++)
    {
        if (m_extObjects[ix1].second) delete m_extObjects[ix1].second;
    }
}


//! Create and return a new object.
/*!
  This function will create a new object and add it to the object list. It will then return an instance of that object to the caller. The function returns NULL if the object was not created. Since the Object Manager is the only class that can create objects all other classes must call this particular function to request a new object.
  @param objType The type of object that is to be added as a QString
  @param objName The name of the object to be added. All objects have names.
  @return An instance of a new object of a particular type. NULL is returned if the object could not be created.
 */
rtRenderObject* rtObjectManager::addObjectOfType(QString objType, QString objName) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::addObjectOfType() start") );
#endif

  rtRenderObject* temp = NULL;
  rtDataObject* dataO = NULL;
  int nextID;

  // Lock this function
  QMutexLocker locker(&m_objectLock);

  // Find out which object will be used.

  if (objType == "OT_None")
  {
      temp=new rtNoneRenderObject();
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Warning: None Object Requested. "));
  } else if (objType == "OT_3DObject")
  {
      temp=new rt3DVolumeRenderObject();
  } else if (objType == "OT_2DObject")
  {
      temp=new rt2DSliceRenderObject();
  } else if (objType == "OT_Cath")
  {
      temp=new rtCathRenderObject();
  } else if (objType == "OT_vtkMatrix4x4")
  {
      temp=new rtMatrixRenderObject();
  } else if (objType == "OT_vtkPolyData")
  {
      temp=new rtPolyRenderObject();
  } else if (objType == "OT_vtkPiecewiseFunction")
  {
      temp=new rtPieceFuncRenderObject();
  } else if (objType == "OT_vtkColorTransferFunction")
  {
      temp=new rtColorFuncRenderObject();
  } else if (objType == "OT_ImageBuffer")
  {
      temp=new rtImageBufferRenderObject();
  } else if (objType == "OT_2DPointBuffer")
  {
      temp=new rt2DPointRenderObject();
  } else if (objType == "OT_3DPointBuffer")
  {
      temp=new rt3DPointBufferRenderObject();
  } else if (objType == "OT_TextLabel")
  {
      temp=new rtLabelRenderObject();
  } else
  {
      bool ok = false;
      for (int ix1=0; ix1<m_extObjects.size(); ix1++)
      {
          if (m_extObjects[ix1].first == objType)
          {
              temp = m_extObjects[ix1].second->createObject();
              ok = true;
          }
      }
      if (!ok)
      {
          rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("No object of type: ").append(objType));
          temp=NULL;
      }
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
    updateGuiObjectList();

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

//! Add an exisiting rtRenderObjectMaker into Vurtigo
/*!
  This function will add the given object to the external object list which will be used to create instances of those objects when needed.
  @param newType The new object type to be added
  @param newObj The object maker that is to be added.
 */
void rtObjectManager::addNewObject(QString newType,rtRenderObjectMaker *newObj)
{
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::addNewObject() start") );
#endif

  if (!newObj)
  {
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Warning: newObj is NULL "));
      return;
  }

  if (!m_objectTypeList.contains(newType))
  {
      m_extObjects.append(qMakePair(newType,newObj));
      m_objectTypeList.append(newType);
      rtApplication::instance().getMainWinHandle()->addNewObjectType(newType);
      updateGuiObjectList();
  }
  else
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Warning: This type already exists "));

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtObjectManager::addNewObject() end") );
#endif
}

//! Create and return a new read-only object.
/*!
  Create a new read-only object of a particular type. This function should only be called by the base and not by any plugin. To create a read-only object the base will first create a R/W object and then set the Read-Only flag.
  @see addObjectOfType()
  @return An instance of a new object of a particular type. NULL is returned if the object could not be created.
 */
rtRenderObject* rtObjectManager::addReadOnlyObject(QString objType, QString objName) {
  rtRenderObject* newObj;

  newObj = addObjectOfType(objType, objName);
  if (newObj) newObj->getDataObject()->setReadOnly();
  return newObj;
}

//! Rename an existing object
void rtObjectManager::renameObjectWithID(int objID, QString newName)
{
    getObjectWithID(objID)->getDataObject()->setObjName(newName);
    updateGuiObjectList();
    emit objectRenamed(objID);
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
    if (!temp || temp->getDataObject()->isReadOnly() || isPermanent(objID)) return false;
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
    if (!temp || isPermanent(objID)) return false;
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
QList<int> rtObjectManager::getObjectsOfType(QString objType) {
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
int rtObjectManager::getNumObjectsOfType(QString objType) {
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

void rtObjectManager::updateGuiObjectList() {
  if ( rtApplication::instance().getMainWinHandle() )
    rtApplication::instance().getMainWinHandle()->updateObjectList(&m_objectHash);
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
