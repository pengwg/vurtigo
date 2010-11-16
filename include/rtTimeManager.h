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
