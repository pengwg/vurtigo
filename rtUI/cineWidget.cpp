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
