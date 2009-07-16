#include "rtTimeManager.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"
#include "rtPluginLoader.h"
#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtObjectManager.h"

#include <iostream>

#include <QTimer>

//! Constructor
rtTimeManager::rtTimeManager() {
  m_renderTime = new QTimer();
  m_pluginUpdateTime = new QTimer();
  m_planeUpdateTime = new QTimer();

  // Fire every 40 mils or 25 fps.
  m_renderTime->setInterval(40);

  // About 40 fps
  m_pluginUpdateTime->setInterval(25);

  // About 30 fps.
  m_planeUpdateTime->setInterval(34);

  connect(m_pluginUpdateTime, SIGNAL(timeout()), this, SLOT(pluginUpdate()));
  connect(m_planeUpdateTime, SIGNAL(timeout()), this, SLOT(planeUpdate()));

  m_pluginUpdateTime->start();
  m_planeUpdateTime->start();

  m_mainWin = NULL;

  for (int ix1=0; ix1<10; ix1++) {
    m_renderTimeBuffer[ix1] = 1.0;
  }
  m_currentSum = 10.0;
  m_renderTimePosition = 0;
  m_frameRateLabel = NULL;
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

    calcFrameRate();
  }
}

//! Do the frame rate calculations.
void rtTimeManager::calcFrameRate() {
  double frameRate;

  if (!m_frameRateLabel)
    m_frameRateLabel = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "Frame Rate");

  frameRate = 1.0f/m_mainWin->getRenderer()->GetLastRenderTimeInSeconds();
  m_currentSum -= m_renderTimeBuffer[m_renderTimePosition];
  m_currentSum += frameRate;
  m_renderTimeBuffer[m_renderTimePosition] = frameRate;
  m_renderTimePosition = (m_renderTimePosition+1) % 10;

  // Update only every thenth.
  if (m_renderTimePosition == 0) {
    static_cast<rtLabelDataObject*>(m_frameRateLabel->getDataObject())->setText(QString("  ").append(QString::number(m_currentSum/10.0f).append(QString(" FPS "))));
    static_cast<rtLabelDataObject*>(m_frameRateLabel->getDataObject())->getTextProperty()->SetLineOffset(4);
    static_cast<rtLabelDataObject*>(m_frameRateLabel->getDataObject())->Modified();
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
  bool renderNew=false;

  for (ix1=0; ix1<m_watchList.size(); ix1++) {
    renderNew = m_watchList.at(ix1)->tryUpdate() || renderNew;
  }

  if (renderNew && m_mainWin) {
    m_mainWin->setRenderFlag3D(true);
  }

}

void rtTimeManager::planeUpdate() {
  if (!m_mainWin) return;
  m_mainWin->update2DViews();
}
