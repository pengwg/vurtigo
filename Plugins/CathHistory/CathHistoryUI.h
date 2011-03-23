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
#ifndef CATH_HISTORY_UI_H
#define CATH_HISTORY_UI_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QFileInfo>
#include <ui_CathHistory.h>

#include "HistoryData.h"
#include "rt3dPointBufferDataObject.h"
#include "rtCathDataObject.h"

class CathHistoryUI : public QWidget, public Ui::CathTracking
{
  Q_OBJECT

public:
  CathHistoryUI();
  ~CathHistoryUI();

public slots:
  void objectAdded(int);
  void objectRemoved(int);
  void pointSizeChanged(int);

  void cathChanged(int);
  void mark();
  void markNow();
  void pointsChanged(int num) {m_numPoints = num;}
  void intervalChanged(int interval) {m_interval = interval;}
  
  void cathAutoTrackChanged(bool);
  void autoSaveChanged(bool);

protected:
  void connectSignals();
  void populateLists();
  void updateCheckableStatus();

  void trackingPairChanged();

  void savePointObject();

  //! Map the combo box indices to the plane objects.
  QMap<int, rtCathDataObject*> m_cathObjectMap;

  //! The ID of the 3DPointBuffer Object we are connected to
  int m_points;

  //! Number of points in a marking set
  int m_numPoints;

  //! Interval between point marks in milliseconds
  int m_interval;

  //! A timer to count the interval between points in a marked set
  QTimer *m_timer;

  //! A counter used to keep track of points in a set
  int m_counter;

  //! The current set
  int m_set;

  //! The cath history data object
  HistoryData *m_historyRecorder;

};

#endif
