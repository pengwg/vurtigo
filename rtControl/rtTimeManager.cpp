#include "rtTimeManager.h"
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"

#include <iostream>

#include <QTimer>

//! Constructor
rtTimeManager::rtTimeManager() {
  m_renderTime = new QTimer();

  // Fire every 34 mils or 30 fps. 
  m_renderTime->setInterval(34); 

  m_mainWin = NULL;
}

//! Destructor
rtTimeManager::~rtTimeManager() {
  if (m_renderTime) {
    m_renderTime->stop();
    m_renderTime->deleteLater();
  }
}

void rtTimeManager::startRenderTimer(rtMainWindow* mainWin, int delay) {
  if (!mainWin) return;

  m_mainWin = mainWin;
  m_renderTime->setInterval(delay);
  connect(m_renderTime, SIGNAL(timeout()), this, SLOT(renderTimeout()));

  m_renderTime->start();
  
}


void rtTimeManager::renderTimeout() {
  if (m_mainWin) {
    checkWatchList();
    m_mainWin->tryRender3D();
  }
}

void rtTimeManager::addToWatchList(rtRenderObject* obj) {
  if(!m_watchList.contains(obj)) {
    m_watchList.append(obj);
  }
}

bool rtTimeManager::isInWatchList(rtRenderObject* obj) {
  return m_watchList.contains(obj);
}

void rtTimeManager::removeFromWatchList(rtRenderObject* obj) {
  int index;
  index = m_watchList.indexOf(obj);

  if (index >= 0) m_watchList.removeAt(index);
}

//! Try to update every object on the watch list.
void rtTimeManager::checkWatchList() {
  int ix1;

  for (ix1=0; ix1<m_watchList.size(); ix1++) {
    m_watchList.at(ix1)->update();
  }
}
