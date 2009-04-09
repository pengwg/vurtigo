#ifndef RT_TIME_MANAGER_H
#define RT_TIME_MANAGER_H

#include <QObject>
#include <QList>

class QTimer;

class rtObjectManager;
class rtMainWindow;
class rtRenderObject;

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

  void addToWatchList(rtRenderObject* obj);
  bool isInWatchList(rtRenderObject* obj);
  void removeFromWatchList(rtRenderObject* obj);

  void checkWatchList();

 public slots:
  void renderTimeout();

 protected:
  //! The timer that services the renderer.
  QTimer *m_renderTime;

  //! Handle to the main window.
  rtMainWindow* m_mainWin;

  //! Objects that are updated at every iteration.
  QList<rtRenderObject*> m_watchList;
 private:
 
};

#endif
