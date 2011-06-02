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
  void addObjectType(QString type);

  //! Remove a type of object from this list.
  void removeObjectType(QString type);

  //! Return the ID of the current object.
  /*!
    \return The id of the currenly selected object. Returns -1 if no object is selected.
    */
  inline int getCurrentObjectId() { return m_currentObjectIndex; }

public slots:
  void objectCreated(int id);
  void objectRemoved(int id);
  void objectRenamed(int id);
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
  QList<QString> m_objectTypes;

  int m_currentObjectIndex;
};

#endif // OBJECTSELECTIONCOMBOBOX_H
