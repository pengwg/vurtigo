#ifndef RT_OBJECT_MANAGER_H
#define RT_OBJECT_MANAGER_H

#include <QHash>
#include "rtMainWindow.h"
#include "rtRenderObject.h"

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
