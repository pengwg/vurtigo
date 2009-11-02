/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "rtTimeManager.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"
#include "rtPluginLoader.h"
#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtObjectManager.h"

#include <iostream>
#include <cmath>

#include <QTimer>

//! Constructor
rtTimeManager::rtTimeManager() {
  m_renderTime = new QTimer();
  m_pluginUpdateTime = new QTimer();
  m_planeUpdateTime = new QTimer();

  m_estimationLen = 50;
  m_renderTimeBuffer.reserve(m_estimationLen);
  m_renderTimeBuffer.resize(m_estimationLen, 0.0f);

  // Fire every 40 mils or 25 fps.
  m_renderTime->setInterval(10);

  // About 40 fps
  m_pluginUpdateTime->setInterval(10);

  // About 30 fps.
  m_planeUpdateTime->setInterval(10);

  connect(m_pluginUpdateTime, SIGNAL(timeout()), this, SLOT(pluginUpdate()));
  connect(m_planeUpdateTime, SIGNAL(timeout()), this, SLOT(planeUpdate()));

  m_pluginUpdateTime->start();
  m_planeUpdateTime->start();

  m_mainWin = NULL;

  m_currentSum = 0.0;
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
  double avgFrameRate;
  double tempSum;
  double stdev;
  QString text = "   ";

  if (!m_frameRateLabel)
    m_frameRateLabel = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "Frame Rate");

  frameRate = 1.0f/m_mainWin->getRenderer()->GetLastRenderTimeInSeconds();
  m_currentSum -= m_renderTimeBuffer[m_renderTimePosition];
  m_currentSum += frameRate;
  m_renderTimeBuffer[m_renderTimePosition] = frameRate;
  m_renderTimePosition = (m_renderTimePosition+1) % m_estimationLen;



  // Update only every nth.
  if (m_renderTimePosition == 0) {
    avgFrameRate = m_currentSum/((double)m_estimationLen);

    tempSum = 0.0f;
    for (int ix1=0; ix1<m_estimationLen; ix1++) {
      tempSum += (avgFrameRate-m_renderTimeBuffer[ix1])*(avgFrameRate-m_renderTimeBuffer[ix1]);
    }
    tempSum = tempSum / ((double)m_estimationLen);
    stdev = sqrt(tempSum);

    text.append(QString::number(avgFrameRate));
    text.append("  (+ or -)  ");
    text.append(QString::number(stdev));
    text.append(" FPS ");

    static_cast<rtLabelDataObject*>(m_frameRateLabel->getDataObject())->setText(text);
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

    // Adjust the rendering quality.
    if (m_mainWin) {
      if (m_mainWin->cameraMoving()) {
        // Camera is moving implies lower quality.
        m_watchList.at(ix1)->setRenderQuality(m_mainWin->getMovingQuality());
      } else {
        m_watchList.at(ix1)->setRenderQuality(m_mainWin->getStillQuality());
      }
    }
  }

  if (renderNew && m_mainWin) {
    m_mainWin->getRenderer()->ResetCameraClippingRange();
    m_mainWin->setRenderFlag3D(true);
  }

}

void rtTimeManager::planeUpdate() {
  if (!m_mainWin) return;
  m_mainWin->update2DViews();
}
