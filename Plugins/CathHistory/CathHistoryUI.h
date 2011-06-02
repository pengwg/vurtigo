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
  void trackingPairChanged();
  void mark();
  void markNow();
  void pointsChanged(int num) {m_numPoints = num;}
  void intervalChanged(int interval) {m_interval = interval;}
  
  void cathAutoTrackChanged(bool);
  void autoSaveChanged(bool);

  void resetPoints();

  void savedObject();

protected:
  void connectSignals();
  void populateLists();
  void updateCheckableStatus();

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

  //! A counter used to keep track of point save attempts
  int m_numTries;

  //! The cath history data object
  HistoryData *m_historyRecorder;

  //! the id of the currently selected catheter
  int m_currCath;

};

#endif
