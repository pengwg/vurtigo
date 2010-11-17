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
  
  void cathAutoTrackChanged(bool);

protected:
  void connectSignals();
  void populateLists();
  void updateCheckableStatus();

  void trackingPairChanged();

  //! Map the combo box indices to the plane objects.
  QMap<int, rtCathDataObject*> m_cathObjectMap;

  int m_points;
  
  HistoryData *m_historyRecorder;

};

#endif
