#ifndef RT_OBJECT_MANAGER_H
#define RT_OBJECT_MANAGER_H

#include <QHash>
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

 protected:
  rtMainWindow *m_mainWinHandle;
  QHash<int, rtRenderObject*> m_objectHash;

 private:

};

#endif
