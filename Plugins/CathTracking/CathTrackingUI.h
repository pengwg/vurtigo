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


  QList<TrackData*> m_trackList;

};

#endif
