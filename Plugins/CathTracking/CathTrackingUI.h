#ifndef CATH_TRACKING_UI_H
#define CATH_TRACKING_UI_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <ui_CathTracking.h>

#include "rt2dSliceDataObject.h"
#include "rtCathDataObject.h"

class CathTrackingUI : public QWidget, public Ui::CathTracking
{
  Q_OBJECT

public:
  CathTrackingUI();
  ~CathTrackingUI();

public slots:
  void objectAdded(int);
  void objectRemoved(int);

  void cathChanged(int);
  void planeChanged(int);

  void trackLocationChanged(int);
  void trackOffsetChanged(double);

protected:
  void connectSignals();
  void populateLists();

  //! Map the combo box indices to the catheter objects.
  QMap<int, rt2DSliceDataObject*> m_planeObjectMap;

  //! Map the combo box indices to the plane objects.
  QMap<int, rtCathDataObject*> m_cathObjectMap;


};

#endif
