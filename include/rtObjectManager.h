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
  int getNumObjectsOfType(QString objType);

  //! Set an object to be permanent (cannot be deleted)
  void setPermanent(int objID) { if (!m_permanentObjects.contains(objID)) m_permanentObjects.append(objID); }
  //! Is an object permanent?
  inline bool isPermanent(int objID) {return m_permanentObjects.contains(objID); }

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

