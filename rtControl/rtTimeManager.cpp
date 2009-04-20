#include "rtTimeManager.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"
#include "rtPluginLoader.h"

#include <iostream>

#include <QTimer>

//! Constructor
rtTimeManager::rtTimeManager() {
  m_renderTime = new QTimer();
  m_pluginUpdateTime = new QTimer();

  // Fire every 34 mils or 30 fps.
  m_renderTime->setInterval(34); 

  // About 40 fps
  m_pluginUpdateTime->setInterval(25);

  connect(m_pluginUpdateTime, SIGNAL(timeout()), this, SLOT(pluginUpdate()));

  m_pluginUpdateTime->start();

  m_mainWin = NULL;
}

//! Destructor
rtTimeManager::~rtTimeManager() {
  if (m_renderTime) {
    m_renderTime->stop();
    m_renderTime->deleteLater();
  }

  if (m_pluginUpdateTime) {
    m_pluginUpdateTime->stop();
    m_pluginUpdateTime->deleteLater();
  }
}

//! Start the timer for the main window. Pass in the handle to the main window.
void rtTimeManager::startRenderTimer(rtMainWindow* mainWin, int delay) {
  if (!mainWin) return;

  m_mainWin = mainWin;
  m_renderTime->setInterval(delay);
  connect(m_renderTime, SIGNAL(timeout()), this, SLOT(renderTimeout()));

  m_renderTime->start();
  
}

//! Function called by a timer to update the render window.
void rtTimeManager::renderTimeout() {
  if (m_mainWin) {
    checkWatchList();
    m_mainWin->tryRender3D();
  }
}

//! Function called by a timer to update the plugins that need to be updated.
void rtTimeManager::pluginUpdate() {
  rtPluginLoader::instance().updatePlugins();
}

//! Add a render object to the watch list.
void rtTimeManager::addToWatchList(rtRenderObject* obj) {
  if(!m_watchList.contains(obj)) {
    m_watchList.append(obj);
  }
}

//! Check if the watch list contains a particular render object.
bool rtTimeManager::isInWatchList(rtRenderObject* obj) {
  return m_watchList.contains(obj);
}

//! Remove a render object from the watch list.
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
