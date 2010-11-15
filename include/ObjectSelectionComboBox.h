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

#ifndef OBJECTSELECTIONCOMBOBOX_H
#define OBJECTSELECTIONCOMBOBOX_H

#include <QComboBox>
#include <QList>
#include "objTypes.h"
#include "rtObjectManager.h"
#include "rtApplication.h"

class ObjectSelectionComboBox : public QComboBox
{
Q_OBJECT
public:
  ObjectSelectionComboBox();

  //! Include a type of object into this list.
  void addObjectType(rtConstants::rtObjectType type);

  //! Remove a type of object from this list.
  void removeObjectType(rtConstants::rtObjectType type);

  //! Return the ID of the current object.
  /*!
    \return The id of the currenly selected object. Returns -1 if no object is selected.
    */
  inline int getCurrentObjectId() { return m_currentObjectIndex; }

public slots:
  void objectCreated(int id);
  void objectRemoved(int id);
  void newSelection(int sel);
  void currObjectModified(int id);

signals:
  //! Signal is emitted when the object id of the selected object changes.
  void objectSelectionChanged(int objId);

  //! Signal is emitted when the currently selected object is modified.
  void selectedObjectModified(int objId);

protected:

  void updateComboList();

  //! List of object types that may appear in this slection box.
  QList<rtConstants::rtObjectType> m_objectTypes;

  int m_currentObjectIndex;
};

#endif // OBJECTSELECTIONCOMBOBOX_H
