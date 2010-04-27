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

#include "objTypes.h"
class rtRenderObject;
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

  rtRenderObject* addObjectOfType(rtConstants::rtObjectType objType, QString objName="Not Named");
  rtRenderObject* addReadOnlyObject(rtConstants::rtObjectType objType, QString objName="Not Named");
  bool removeObject(int objID);
  bool removeReadOnly(int objID);
  rtRenderObject* getObjectWithID(int objID);
  QList<int> getObjectsOfType(rtConstants::rtObjectType objType);
  int getNumObjectsOfType(rtConstants::rtObjectType objType);

  inline QMultiHash<int, QString>* get2DObjectNameHash() { return &m_list2DHash; }

 protected:
  //! Hash table of all the objects listed by unique ID
  QHash<int, rtRenderObject*> m_objectHash;

  //! A list of strings corresponding to the names of all the 2D objects.
  /*!
    The hash is indexed by ID wihich is the unique ID given to the object with that pipeline.
    However, since one object may have multiple 2D renderable objects the ids in this Hash are not unique.
    */
  QMultiHash<int, QString> m_list2DHash;

 signals:
  void objectCreated(int objID);
  void objectRemoved(int objID);

 private:
  //! Maximum number of objects.
  int m_max_object;

  int getNextID();

  rtObjectManager(const rtObjectManager&);
  rtObjectManager& operator=(const rtObjectManager&);
};

#endif
