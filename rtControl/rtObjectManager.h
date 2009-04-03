#ifndef RT_OBJECT_MANAGER_H
#define RT_OBJECT_MANAGER_H

#include <QHash>
#include <QList>
#include "rtMainWindow.h"
#include "rtRenderObject.h"

//!  The Object Manager. 
/*!
  This should be the only class that can create or delete rtRenderObject instances. As a result it can keep track of all such instances and hand them out to other classes when requested. 
*/
class rtObjectManager {
 public:
  rtObjectManager();
  ~rtObjectManager();

  void setMainWinHandle(rtMainWindow* mainWin);
  rtMainWindow* getMainWinHandle();

  rtRenderObject* addObjectOfType(rtConstants::rtObjectType objType, QString objName="Not Named");
  rtRenderObject* addReadOnlyObject(rtConstants::rtObjectType objType, QString objName="Not Named");
  bool removeObject(int objID);
  bool removeReadOnly(int objID);
  rtRenderObject* getObjectWithID(int objID);
  QList<int> getObjectsOfType(rtConstants::rtObjectType objType);
  int getNumObjectsOfType(rtConstants::rtObjectType objType);

 protected:
  //! A pointer to the main window object.
  /*!
    The object manager needs to communicate with the GUI. In general, it updates lists. This hamdle will not be modified by the object manager and should not be modified by any object. It is only meant as a way to modify the GUI. 
  */
  rtMainWindow *m_mainWinHandle;
  QHash<int, rtRenderObject*> m_objectHash;
 private:
  //! Maximum number of objects.
  int m_max_object;

  int getNextID();
};

#endif
