#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include <QObject>
#include "rt2dSliceDataObject.h"
#include "rtCathDataObject.h"
#include "rt3dVolumeDataObject.h"


class TrackData : public QObject
{
  Q_OBJECT

 public:
    TrackData(rtCathDataObject* cath, rt2DSliceDataObject* slice);
    TrackData(rtCathDataObject* cath, rt3DVolumeDataObject* vol);
    ~TrackData();

    //! Check if this trackdata's objects are the same as the ones given
    bool equivalentCPTo(rtCathDataObject* cath, rt2DSliceDataObject* slice);
    //! Check if this trackdata's objects are the same as the ones given
    bool equivalentCVTo(rtCathDataObject* cath, rt3DVolumeDataObject* vol);

    int getLocation() { return m_location; }
    void setLocation(int loc) { m_location = loc; }

    double getOffset() { return m_offset; }
    void setOffest(double offset) { m_offset = offset; }

    bool getAxial() {return m_axial;}
    bool getSagittal() {return m_sagittal;}
    bool getCoronal() {return m_coronal;}

    bool isTracking();
    void setTrackingOn();
    void setTrackingOff();
    void setTracking(bool t);
    
    bool isSliceOnly();
    void setSliceOnly(bool value);

    void setPlanes(bool axial,bool sagittal,bool coronal);

    rt2DSliceDataObject* getSliceObject() { return m_slice; }
    rtCathDataObject* getCathObject() { return m_cath; }
    rt3DVolumeDataObject* getVolObject() {return m_vol; }

 public slots:
    //! Update is called when the catheter starts tracking and when it has moved.
    void update();

 protected:
  int m_location;
  double m_offset;
  bool m_tracking;
  bool m_sliceOnly;
  bool m_axial;
  bool m_sagittal;
  bool m_coronal;
  rt2DSliceDataObject* m_slice;
  rtCathDataObject* m_cath;
  rt3DVolumeDataObject *m_vol;

  // Check if the catheter has really changed.
  int m_old_location;
  double m_old_offset;
  bool m_old_tracking;
  double m_old_position[3];

  double findDistance(double a[3], double b[3]);
};

#endif
