/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "cineWidget.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

#include <QtAlgorithms>

CineWidget::CineWidget()
    : m_currentPhase(0)
{
  m_guiObjects.setupUi(this);

  m_guiObjects.cineFrameLCD->setNumDigits(3);
  m_guiObjects.controlsGroupBox->setEnabled(false);

  m_cineFrameTimer = new QTimer(this);
  connect(m_cineFrameTimer, SIGNAL(timeout()), this, SLOT(nextVisibleComponent()) );

  m_triggerTimes.clear();

  // Connect GUI to slots.
  connect(m_guiObjects.cineBackOne, SIGNAL(clicked()), this, SLOT(backOne()));
  connect(m_guiObjects.cineForwardOne, SIGNAL(clicked()), this, SLOT(forwardOne()));
  connect(m_guiObjects.cineFirst, SIGNAL(clicked()), this, SLOT(goToFirst()));
  connect(m_guiObjects.cineLast, SIGNAL(clicked()), this, SLOT(goToLast()));
  connect(m_guiObjects.cineLoop, SIGNAL(clicked()), this, SLOT(loopButtonChanged()));
  connect(m_guiObjects.cineSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
}

CineWidget::~CineWidget() {
  if (m_cineFrameTimer && m_cineFrameTimer->isActive()) {
    m_cineFrameTimer->stop();
  }
  if (m_cineFrameTimer) delete m_cineFrameTimer;

  m_triggerTimes.clear();
}


void CineWidget::setTriggerList(QList<int> trigList) {
  m_triggerTimes = trigList;

  // Check if there is more than one frame.
  if (m_triggerTimes.size() > 1) {
    qSort(m_triggerTimes.begin(), m_triggerTimes.end());
    m_guiObjects.controlsGroupBox->setEnabled(true);
    m_currentPhase = 0;

    m_guiObjects.cineFrameLCD->display(m_currentPhase);
    m_guiObjects.cineSlider->setMinimum(0);
    m_guiObjects.cineSlider->setMaximum(m_triggerTimes.size()-1);
    m_guiObjects.cineSlider->setValue(m_currentPhase);

  } else {
    m_guiObjects.controlsGroupBox->setEnabled(false);
  }
}

void CineWidget::addTrigger(int trig) {
  m_triggerTimes.append(trig);

  if (m_triggerTimes.size() > 1) {
    qSort(m_triggerTimes.begin(), m_triggerTimes.end());
    m_guiObjects.controlsGroupBox->setEnabled(true);
    m_currentPhase = 0;

    m_guiObjects.cineFrameLCD->display(m_currentPhase);
    m_guiObjects.cineSlider->setMinimum(0);
    m_guiObjects.cineSlider->setMaximum(m_triggerTimes.size()-1);
    m_guiObjects.cineSlider->setValue(m_currentPhase);

  } else {
    m_guiObjects.controlsGroupBox->setEnabled(false);
  }
}

///////////////
// Slots
///////////////

void CineWidget::backOne() {
  if (m_triggerTimes.size() <= 1) return;

  if (m_currentPhase > 0) {
    m_guiObjects.cineSlider->setValue(m_currentPhase-1);
  }
}

void CineWidget::forwardOne(){
  if (m_triggerTimes.size() <= 1) return;

  if (m_currentPhase < m_triggerTimes.size()-1) {
    m_guiObjects.cineSlider->setValue(m_currentPhase+1);
  }
}

void CineWidget::goToFirst() {
  if (m_triggerTimes.size() <= 1) return;
  if (m_currentPhase != 0) {
    m_guiObjects.cineSlider->setValue(0);
  }
}

void CineWidget::goToLast() {
  if (m_triggerTimes.size() <= 1) return;
  if (m_currentPhase != m_triggerTimes.size()-1) {
    m_guiObjects.cineSlider->setValue(m_triggerTimes.size()-1);
  }
}

void CineWidget::loopButtonChanged() {
  if (m_triggerTimes.size() <= 1) return;

  if (m_guiObjects.cineLoop->isChecked()) {
    // Turn on the loop for cine.
    m_cineFrameTimer->start(20);
  } else if (m_cineFrameTimer->isActive()) {
    // Turn off the loop for cine.
    m_cineFrameTimer->stop();
  }
}

void CineWidget::sliderValueChanged(int val) {
  if (m_triggerTimes.size() <= 1) return;
  if (m_currentPhase == val) return;

  if (val > m_triggerTimes.size()-1) val = m_triggerTimes.size()-1;
  if (val < 0) val = 0;

  m_currentPhase = val;
  m_guiObjects.cineFrameLCD->display(m_currentPhase);
  m_guiObjects.cineSlider->setValue(m_currentPhase);
  emit phaseChanged(m_currentPhase);
  emit triggerChanged(m_triggerTimes.at(m_currentPhase));
}

void CineWidget::nextVisibleComponent() {
  if (m_triggerTimes.size() <= 1) return;

  int trig = rtApplication::instance().getTimeManager()->getTriggerDelay();
  int dist = abs(m_triggerTimes.at(0)-trig);
  int phase = 0;
  for (int ix1=1; ix1<m_triggerTimes.size(); ix1++) {
    if (abs(m_triggerTimes.at(ix1)-trig) < dist) {
      dist = abs(m_triggerTimes.at(ix1)-trig);
      phase = ix1;
    }
  }
  m_guiObjects.cineSlider->setValue(phase);
}
