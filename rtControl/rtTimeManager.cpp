#include "rtTimeManager.h"
#include <iostream>

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
    m_mainWin->tryRender3D();
  }
}
