/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef RT_OBJECT_MANAGER_H
#define RT_OBJECT_MANAGER_H

#include <QHash>
#include <QList>
#include <QMutex>

#include "objTypes.h"
class rtRenderObject;
class rtRenderObjectMaker;
class rtMainWindow;
class rtDataObject;

#include "vtkProp.h"

//!  The Object Manager.
/*!
  Manages the creation and deletion of objects.
*/
class rtObjectManager : public QObject {
  Q_OBJECT

 public:
  rtObjectManager();
  ~rtObjectManager();

  rtRenderObject* addObjectOfType(QString objType, QString objName="Not Named");
  //! Register a new object type in Vurtigo and store the object to create more if needed
  void addNewObject(QString newType,rtRenderObjectMaker *newObj);
  rtRenderObject* addReadOnlyObject(QString objType, QString objName="Not Named");
  void renameObjectWithID(int objID, QString newName);
  bool removeObject(int objID);
  bool removeReadOnly(int objID);
  rtRenderObject* getObjectWithID(int objID);
  QList<int> getObjectsOfType(QString objType);
  QList<rtRenderObject*> getAllObjects();
  int getNumObjectsOfType(QString objType);

  //! Set an object to be permanent (cannot be deleted)
  void setPermanent(int objID) { if (!m_permanentObjects.contains(objID)) m_permanentObjects.append(objID); }
  //! Is an object permanent?
  inline bool isPermanent(int objID) {return m_permanentObjects.contains(objID); }

  //! Does Vurtigo have this object type?
  bool hasType(QString objType) { return m_objectTypeList.contains(objType); }

  //! Get a pointer to the 2D object hash.
  inline QMultiHash<int, QString>* get2DObjectNameHash() { return &m_list2DHash; }

  //! Force an update of the object list in the GUI.
  void updateGuiObjectList();

  //! Get the next unused ID.
  /*!
    @return The next unused ID
  */
  int getNextID();

 protected:
  //! Hash table of all the objects listed by unique ID
  QHash<int, rtRenderObject*> m_objectHash;

  //! A list of strings corresponding to the names of all the 2D objects.
  /*!
    The hash is indexed by ID wihich is the unique ID given to the object with that pipeline.
    However, since one object may have multiple 2D renderable objects the ids in this Hash are not unique.
    */
  QMultiHash<int, QString> m_list2DHash;

  //! Lock for object creation and deletion.
  /*!
    Only one object should be created or deleted at one time. This ensures that the object list remains consistent.
    */
  QMutex m_objectLock;

  //! A list of object makers added from plugins
  QList<QPair<QString,rtRenderObjectMaker *> > m_extObjects;

  //! A list of object types Vurtigo can create
  QList<QString> m_objectTypeList;
signals:
  void objectCreated(int objID);
  void objectRemoved(int objID);
  void objectRenamed(int objID);

 private:
  //! Maximum number of objects.
  int m_max_object;

  //! A list of objects that cannot be deleted
  QList<int> m_permanentObjects;

  rtObjectManager(const rtObjectManager&);
  rtObjectManager& operator=(const rtObjectManager&);
};

#endif

