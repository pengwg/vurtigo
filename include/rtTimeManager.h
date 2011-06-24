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
#ifndef RT_TIME_MANAGER_H
#define RT_TIME_MANAGER_H

#include <QObject>
#include <QList>
#include <QTime>

#include <vector>

#include "ui_rtTimeOptionsDialog.h"

class QTimer;
class rtRenderObject;

//! Controls timing for the base and all plugins. [Singleton]
/*!
  This is a control class for the whole program. It is important that the objects are timed correctly otherwise some tasks will be starved. This class inherits from QObject because it needs to use signals and slots. 
  Note that this is also a singleton class. 
 */
class rtTimeManager : public QObject {

  Q_OBJECT

 public:
  rtTimeManager();
  ~rtTimeManager();

  void startRenderTimer(int delay=34);
  void addToWatchList(rtRenderObject* obj);
  bool isInWatchList(rtRenderObject* obj);
  void removeFromWatchList(rtRenderObject* obj);
  void checkWatchList();

  //! Get the number of msec elapsed since the start of the application.
  int getSystemTime() { return m_appTime.elapsed(); }

  //! Get the system time as a string as HH:MM:SS
  QString getSystemTimeAsString() { return m_appTime.toString("hh:mm:ss"); }

  //! Sets the global trigger delay.
  /*! This delay will be ignored if the simulation delay is running.
    \sa m_useSimulatedTrigger
    */
  inline void setGlobalTriggerDelay(int delay) { m_globalTriggerDelay=delay; }

  //! Set the global resp
  inline void setGlobalResp(int resp) { m_globalResp = resp; }

  //! Get the trigger delay.
  /*!
    The value returned depends if the mode is in simulated mode [default] or in real mode.
If the m_useSimulatedTrigger is true then the simulated mode is run otherwise in real mode the value of m_globalTriggerDelay is returned.
\sa m_useSimulatedTrigger
\sa m_globalTriggerDelay
\sa setGlobalTriggerDelay
    */
  int getTriggerDelay();

  //! Get the global value for the respiratory.
  inline int getGlobalResp() { return m_globalResp; }

  //! Get the phase number given that there are n total phases.
  int getPhaseForNumPhases(int n);

  //! If we are using simulated triggers
  bool getSimulatedTrigger() {return m_useSimulatedTrigger; }

 public slots:
  //! Time to update the render window.
  void renderTimeout();

  //! Time to update plugins
  void pluginUpdate();

  //! Update the 2D views.
  void planeUpdate();

  //! Show the dialog with all the options for the time manager.
  void showTimeOptionsDialog() { m_timeDialog.show(); }

  void renderTimeChanged(int);
  void pluginTimeChanged(int);
  void planeTimeChanged(int);
  void triggerTimeSourceChanged(bool);
  void cycleLengthChanged(int);
  void cycleSlowdownChanged(int);
 protected:

  //! setup the UI when the object is created.
  void setupUI();

  //! Do the frame rate calculations.
  void calcFrameRate();

  unsigned int m_estimationLen;

  //! The options dialog for the time manager
  Ui::rtTimeOptionsDialog m_dialogOptions;
  QDialog m_timeDialog;

  //! The cardiac trigger delay to be used for the whole application.
  int m_globalTriggerDelay;

  //! The global respiratory value to be used for the whole application.
  int m_globalResp;

  //! The application time.
  QTime m_appTime;

  //! The length of the cardiac cycle in ms.
  int m_cardiacCycleLength;

  //! A slowdown factor for the cardiac simulation cycle.
  /*!
    A slowdown factor in the range (0, 1) increases the speed of the cardiac cycle.
    A slowdown factor of 1.0 maintains the speed of the cycle at real time. [default]
    Any factor greater than 1.0 slows the rate down.
    For example, for a factor of 2.0, a cardiac cycle of 800 ms will be played out in 1600 ms but the trigger delays will still range over [0, 800].
This is useful for recording of videos after an experiment when the renderer cannot keep up with the processing required to record one frame every phase.
    */
  double m_cardiacCycleSlowdown;

  //! Use the simulated trigger times.
  bool m_useSimulatedTrigger;

  //! The timer that services the renderer.
  QTimer *m_renderTime;
  //! Timer to check the plugins for updates
  QTimer *m_pluginUpdateTime;
  //! Timer to check the 2D planes for updates.
  QTimer *m_planeUpdateTime;

  //! Objects that are updated at every iteration.
  QList<rtRenderObject*> m_watchList;

  // Render Time Estimation
  std::vector<double> m_renderTimeBuffer;
  double m_currentSum;
  int m_renderTimePosition;

  //! Text label used to display the frame rate.
  rtRenderObject* m_frameRateLabel;

 private:
  rtTimeManager(const rtTimeManager&);
  rtTimeManager& operator=(const rtTimeManager&);
};

#endif
