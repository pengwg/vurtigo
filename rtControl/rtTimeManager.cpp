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
#include "buildParam.h"
#include "rtMessage.h"
#include "rtApplication.h"

#include <iostream>
#include <cmath>

#include <QTimer>

//! Constructor
rtTimeManager::rtTimeManager() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::rtTimeManager() start") );
#endif
  m_renderTime = new QTimer();
  m_pluginUpdateTime = new QTimer();
  m_planeUpdateTime = new QTimer();

  m_dialogOptions.setupUi(&m_timeDialog);

  m_estimationLen = 50;
  m_renderTimeBuffer.reserve(m_estimationLen);
  m_renderTimeBuffer.resize(m_estimationLen, 0.0f);

  m_renderTime->setInterval(10);
  m_pluginUpdateTime->setInterval(10);
  m_planeUpdateTime->setInterval(10);

  connect(m_pluginUpdateTime, SIGNAL(timeout()), this, SLOT(pluginUpdate()));
  connect(m_planeUpdateTime, SIGNAL(timeout()), this, SLOT(planeUpdate()));

  m_pluginUpdateTime->start();
  m_planeUpdateTime->start();

  m_currentSum = 0.0;
  m_renderTimePosition = 0;
  m_frameRateLabel = NULL;

  m_useSimulatedTrigger = true;
  m_cardiacCycleSlowdown = 1.0f;
  m_cardiacCycleLength = 800;
  m_globalTriggerDelay = 0;
  m_globalResp = 0;

  m_appTime.start();

  // Setup some of the default GUI parameters.
  setupUI();


#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::rtTimeManager() end") );
#endif
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
void rtTimeManager::startRenderTimer(int delay) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::startRenderTimer() start") );
#endif

  m_renderTime->setInterval(delay);
  connect(m_renderTime, SIGNAL(timeout()), this, SLOT(renderTimeout()));
  m_renderTime->start();

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::startRenderTimer() end") );
#endif
}

//! Function called by a timer to update the render window.
void rtTimeManager::renderTimeout() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::renderTimeout() start") );
#endif

  if ( rtApplication::instance().getMainWinHandle() ) {
    checkWatchList();
    rtApplication::instance().getMainWinHandle()->tryRender3D();

    calcFrameRate();
  } else {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtTimeManager::renderTimeout() Timeout received but window is still NULL ") );
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::renderTimeout() end") );
#endif
}



//! Function called by a timer to update the plugins that need to be updated.
void rtTimeManager::pluginUpdate() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::pluginUpdate() start") );
#endif
  rtApplication::instance().getPluginLoader()->updatePlugins();
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::pluginUpdate() end") );
#endif
}

//! Add a render object to the watch list.
void rtTimeManager::addToWatchList(rtRenderObject* obj) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::addToWatchList() start") );
#endif
  if(!m_watchList.contains(obj)) {
    m_watchList.append(obj);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::addToWatchList() end") );
#endif
}

//! Check if the watch list contains a particular render object.
bool rtTimeManager::isInWatchList(rtRenderObject* obj) {
  return m_watchList.contains(obj);
}

//! Remove a render object from the watch list.
void rtTimeManager::removeFromWatchList(rtRenderObject* obj) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::removeFromWatchList() start") );
#endif
  int index=-1;
  index = m_watchList.indexOf(obj);

  if (index >= 0) m_watchList.removeAt(index);
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::removeFromWatchList() end") );
#endif
}

//! Try to update every object on the watch list.
void rtTimeManager::checkWatchList() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::checkWatchList() start") );
#endif
  int ix1=0;
  bool renderNew=false;

  for (ix1=0; ix1<m_watchList.size(); ix1++) {
    if (!m_watchList.at(ix1)) {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtTimeManager::checkWatchList() Watch List Object is NULL at position: ").append(QString::number(ix1)) );
      continue;
    }

    renderNew = m_watchList.at(ix1)->tryUpdate() || renderNew;

    for (int ix2=0; ix2<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix2++)
    {
    // Adjust the rendering quality.
        if (rtApplication::instance().getMainWinHandle()) {
            if (rtApplication::instance().getMainWinHandle()->cameraMoving(ix2)) {
                // Camera is moving implies lower quality.
                m_watchList.at(ix1)->setRenderQuality(rtApplication::instance().getMainWinHandle()->getMovingQuality());
            } else {
                m_watchList.at(ix1)->setRenderQuality(rtApplication::instance().getMainWinHandle()->getStillQuality());
            }
        }
    }
  }

  for (int ix2=0; ix2<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix2++)
  {
      if (renderNew && rtApplication::instance().getMainWinHandle()) {
          rtApplication::instance().getMainWinHandle()->getRenderer(ix2)->ResetCameraClippingRange();
          rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
      }
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::checkWatchList() end") );
#endif
}

void rtTimeManager::planeUpdate() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::planeUpdate() start") );
#endif
  if (!rtApplication::instance().getMainWinHandle()) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtTimeManager::planeUpdate() Main window pointer is NULL. Update skipped. ") );
    return;
  }
  rtApplication::instance().getMainWinHandle()->update2DViews();
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtTimeManager::planeUpdate() end") );
#endif
}

int rtTimeManager::getTriggerDelay() {
  if (m_useSimulatedTrigger) {
    int slowdownLength;
    slowdownLength = ceil(((double)m_cardiacCycleLength)*m_cardiacCycleSlowdown);
    return ((double)(m_appTime.elapsed() % slowdownLength) )/m_cardiacCycleSlowdown;
  } else {
    return m_globalTriggerDelay;
  }
}

int rtTimeManager::getPhaseForNumPhases(int n) {
  double phaseTime = ((double)m_cardiacCycleLength)/((double)n);
  int phase = getTriggerDelay()/phaseTime;
  return phase;
}

void rtTimeManager::renderTimeChanged(int t) {
  m_dialogOptions.renderUpdateSlider->setValue(t);
  m_dialogOptions.renderUpdateLabel->setText(QString::number(t).append(" ms"));
  m_renderTime->setInterval(t);
}

void rtTimeManager::pluginTimeChanged(int t) {
  m_dialogOptions.pluginUpdateSlider->setValue(t);
  m_dialogOptions.pluginUpdateLabel->setText(QString::number(t).append(" ms"));
  m_pluginUpdateTime->setInterval(t);
}

void rtTimeManager::planeTimeChanged(int t) {
  m_dialogOptions.planeUpdateSlider->setValue(t);
  m_dialogOptions.planeUpdateLabel->setText(QString::number(t).append(" ms"));
  m_planeUpdateTime->setInterval(t);
}

void rtTimeManager::triggerTimeSourceChanged(bool src) {
  m_useSimulatedTrigger = src;
  m_dialogOptions.triggerTimesGroupBox->setChecked(m_useSimulatedTrigger);
}

void rtTimeManager::cycleLengthChanged(int len) {
  m_dialogOptions.cycleLengthSlider->setValue(len);
  m_dialogOptions.cycleLengthLabel->setText(QString::number(len).append(" ms"));
  m_cardiacCycleLength = len;
}

void rtTimeManager::cycleSlowdownChanged(int len) {
  double factor;
  factor = (double)len / 1000.0f;

  m_dialogOptions.cycleSlowdownSlider->setValue(len);
  m_dialogOptions.cycleSlowdownLabel->setText(QString::number(factor).append(" "));
  m_cardiacCycleSlowdown = factor;
}

////////////////
// Protected Functions
/////////////////
void rtTimeManager::setupUI() {

  m_timeDialog.setModal(true);

  renderTimeChanged(m_renderTime->interval());
  pluginTimeChanged(m_pluginUpdateTime->interval());
  planeTimeChanged(m_planeUpdateTime->interval());

  triggerTimeSourceChanged(m_useSimulatedTrigger);

  cycleLengthChanged(m_cardiacCycleLength);
  cycleSlowdownChanged(m_cardiacCycleSlowdown*1000.0f);

  connect(m_dialogOptions.renderUpdateSlider, SIGNAL(valueChanged(int)), this, SLOT(renderTimeChanged(int)));
  connect(m_dialogOptions.pluginUpdateSlider, SIGNAL(valueChanged(int)), this, SLOT(pluginTimeChanged(int)));
  connect(m_dialogOptions.planeUpdateSlider, SIGNAL(valueChanged(int)), this, SLOT(planeTimeChanged(int)));
  connect(m_dialogOptions.triggerTimesGroupBox, SIGNAL(clicked(bool)), this, SLOT(triggerTimeSourceChanged(bool)));
  connect(m_dialogOptions.cycleLengthSlider, SIGNAL(valueChanged(int)), this, SLOT(cycleLengthChanged(int)));
  connect(m_dialogOptions.cycleSlowdownSlider, SIGNAL(valueChanged(int)), this, SLOT(cycleSlowdownChanged(int)));
}


void rtTimeManager::calcFrameRate() {
  double frameRate;
  double avgFrameRate;
  double tempSum;
  double stdev;
  QString text = "   ";

  if (!m_frameRateLabel)
    m_frameRateLabel = rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_TextLabel, "Frame Rate");

  frameRate = 1.0f/rtApplication::instance().getMainWinHandle()->getRenderer()->GetLastRenderTimeInSeconds();
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

    rtLabelDataObject* labelObj;
    labelObj = static_cast<rtLabelDataObject*>(m_frameRateLabel->getDataObject());
    if (labelObj) {
      labelObj->setText(text);
      labelObj->getTextProperty()->SetLineOffset(4);
      labelObj->Modified();
    } else {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtTimeManager::calcFrameRate() Frame Rate Label is NULL.") );
    }
  }
}
