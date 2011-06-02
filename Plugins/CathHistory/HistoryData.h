#ifndef HISTORY_DATA_H
#define HISTORY_DATA_H

#include <QObject>
#include "rtCathDataObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rt3DTimePointData.h"

class HistoryData : public QObject
{
  Q_OBJECT

 public:
    HistoryData(rtCathDataObject* cath, rt3DPointBufferDataObject* points);
    ~HistoryData();

    bool equivalentTo(rtCathDataObject* cath, rt3DPointBufferDataObject* points);
    
    void setCathObject(rtCathDataObject* cath) { m_cath = cath; }
    
    rtCathDataObject* getCathObject() { return m_cath; }
    
    rt3DPointBufferDataObject* getPointBufferObject() { return m_points; }
    
    void setAutoTrack(bool fEnabled);
    
    inline int getPointSize() { return m_pointSize; }
    inline void setPointSize(int sz) { if (sz > 0) m_pointSize = sz; }

 public slots:
    bool savePoint();
    bool saveSetPoint(int set);
    void doAutoTrack();

 protected:
  rtCathDataObject* m_cath;
  rt3DPointBufferDataObject* m_points;

  rt3DTimePointData createPoint(double pos[3]);
  bool getCathPosition(double cathPos[3]);
  
  bool m_fAutoTrack;
  rtBasic3DPointData m_prevAutoTrackPoint;
  float m_autoTrackDistanceThreshold;

  int m_pointSize;
  
};

#endif
