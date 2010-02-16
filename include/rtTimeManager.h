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

  //! Get the trigger delay.
  int getTriggerDelay() { return m_appTime.elapsed() % m_cardiacCycleLength; }

  //! Get the phase number given that there are n total phases.
  int getPhaseForNumPhases(int n);

 public slots:
  void renderTimeout();
  void pluginUpdate();

  //! Update the 2D views.
  void planeUpdate();

 protected:
  unsigned int m_estimationLen;

  //! The application time.
  QTime m_appTime;

  //! The length of the cardiac cycle in ms.
  int m_cardiacCycleLength;

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
  rtRenderObject* m_frameRateLabel;

  void calcFrameRate();

 private:
  rtTimeManager(const rtTimeManager&);
  rtTimeManager& operator=(const rtTimeManager&);
};

#endif
