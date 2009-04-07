#ifndef RT_TIME_MANAGER_H
#define RT_TIME_MANAGER_H

#include <QTimer>
#include <QObject>

#include "rtObjectManager.h"
#include "rtMainWindow.h"

//! Controls timing for the base and all plugins. 
/*!
  This is a control class for the whole program. It is important that the objects are timed correctly otherwise some tasks will be starved. This class inherits from QObject because it needs to use signals and slots. 
 */
class rtTimeManager : public QObject {

  Q_OBJECT

 public:
  rtTimeManager();
  ~rtTimeManager();

  void startRenderTimer(rtMainWindow* mainWin, int delay=34);

 public slots:
  void renderTimeout();

 protected:
  //! The timer that services the renderer.
  QTimer *m_renderTime;
  rtMainWindow* m_mainWin;
 private:
 
};

#endif
