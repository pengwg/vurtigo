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
#ifndef CATH_TRACKING_UI_H
#define CATH_TRACKING_UI_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <ui_CathTracking.h>

#include "TrackData.h"
#include "rt2dSliceDataObject.h"
#include "rtCathDataObject.h"
#include "rt3dVolumeDataObject.h"
#include "ObjectSelectionComboBox.h"

class CathTrackingUI : public QWidget, public Ui::CathTracking
{
  Q_OBJECT

public:
  CathTrackingUI();
  ~CathTrackingUI();

public slots:
  void cathChanged(int);
  void planeChanged(int);
  void volumeChanged(int);

  void trackLocationChanged(int);
  void trackOffsetChanged(double);

  void trackChanged(bool);
  
  void sliceOnlyChanged(bool);

  void planesChanged();

protected:
  void connectSignals();
  void updateCheckableStatus();
  void trackingPairChanged();

  TrackData* getPairCP(rt2DSliceDataObject*, rtCathDataObject*);
  TrackData* getPairCV(rt3DVolumeDataObject*, rtCathDataObject*);

  // update the status of the tracking pairs
  void updateInfo();

  ObjectSelectionComboBox m_cathComboBox;
  ObjectSelectionComboBox m_planeComboBox;
  ObjectSelectionComboBox m_volumeComboBox;

  //! Map the combo box indices to the catheter objects.
  QMap<int, rt2DSliceDataObject*> m_planeObjectMap;

  //! Map the combo box indices to the plane objects.
  QMap<int, rtCathDataObject*> m_cathObjectMap;

  //! Map the combo box indices to the volume objects
  QMap<int, rt3DVolumeDataObject*> m_volumeObjectMap;

  QList<TrackData*> m_trackList;

};

#endif
