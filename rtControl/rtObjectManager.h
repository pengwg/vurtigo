#ifndef RT_OBJECT_MANAGER_H
#define RT_OBJECT_MANAGER_H

#include "rtMainWindow.h"

class rtObjectManager {
 public:
  rtObjectManager();
  ~rtObjectManager();

 protected:
  rtMainWindow *m_mainWinHandle;
 private:
};

#endif
