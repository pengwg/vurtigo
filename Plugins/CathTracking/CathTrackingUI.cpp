#include "CathTrackingUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"
#include "rtApplication.h"

#include <QList>

CathTrackingUI::CathTrackingUI() {
  setupUi(this);

  m_cathComboBox.addObjectType("OT_Cath");
  m_planeComboBox.addObjectType("OT_2DObject");
  m_volumeComboBox.addObjectType("OT_3DObject");

  this->gridLayout->addWidget(&m_cathComboBox, 0, 1, 1, 2);
  this->gridLayout->addWidget(&m_planeComboBox, 1, 1, 1, 2);
  this->gridLayout->addWidget(&m_volumeComboBox,2, 1, 1, 2);

  m_trackList.clear();

  connectSignals();
  updateCheckableStatus();
}

CathTrackingUI::~CathTrackingUI() {
  TrackData* temp;
  while(m_trackList.count() > 0) {
    temp = m_trackList.takeAt(0);
    delete temp;
  }
}

void CathTrackingUI::connectSignals() {

  connect( &m_cathComboBox, SIGNAL(objectSelectionChanged(int)), this, SLOT(cathChanged(int)) );
  connect( &m_planeComboBox, SIGNAL(objectSelectionChanged(int)), this, SLOT(planeChanged(int)) );
  connect( &m_volumeComboBox, SIGNAL(objectSelectionChanged(int)),this, SLOT(volumeChanged(int)));

  connect( trackLocSpinBox, SIGNAL(valueChanged(int)), this, SLOT(trackLocationChanged(int)) );
  connect( trackOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(trackOffsetChanged(double)) );

  connect( trackGroupBox, SIGNAL(toggled(bool)), this, SLOT(trackChanged(bool)));
  
  connect( sliceOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(sliceOnlyChanged(bool)));
  connect( axialCheck, SIGNAL(clicked()),this,SLOT(planesChanged()));
  connect( sagittalCheck, SIGNAL(clicked()),this,SLOT(planesChanged()));
  connect( coronalCheck, SIGNAL(clicked()),this,SLOT(planesChanged()));
}

void CathTrackingUI::updateCheckableStatus() {
  int cathId, planeId, volId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  rt3DVolumeDataObject* volPtr = NULL;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();
  volId = m_volumeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  if (volId >= 0)
  {
      volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
  }

  if (cathPtr && (planePtr || volPtr))
  {
      trackGroupBox->setEnabled(true);
      if (planePtr) { sliceOnlyCheckBox->setEnabled(true);}
      if (volPtr)
      {
          axialCheck->setEnabled(true);
          sagittalCheck->setEnabled(true);
          coronalCheck->setEnabled(true);
      }

  }
  else
  {
      trackGroupBox->setEnabled(false);
      sliceOnlyCheckBox->setEnabled(false);
      axialCheck->setEnabled(false);
      sagittalCheck->setEnabled(false);
      coronalCheck->setEnabled(false);
  }
}

void CathTrackingUI::cathChanged(int newpos) {
  updateCheckableStatus();
  trackingPairChanged();
  updateInfo();

}

void CathTrackingUI::planeChanged(int newpos) {
  updateCheckableStatus();
  trackingPairChanged();
  updateInfo();
}

void CathTrackingUI::volumeChanged(int newpos)
{
    updateCheckableStatus();
    trackingPairChanged();
    updateInfo();
}


void CathTrackingUI::trackingPairChanged() {
  int cathId, planeId, volId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  rt3DVolumeDataObject* volPtr = NULL;
  TrackData* td;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();
  volId = m_volumeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  if (volId >= 0)
  {
      volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
  }

  // Not relevant if at least one does not exist.
  if (cathPtr && (planePtr || volPtr))
  {
      if (planePtr)
      {
          td = getPairCP( planePtr, cathPtr );
          trackGroupBox->setChecked(td->isTracking());
          trackLocSpinBox->setValue(td->getLocation());
          trackOffsetDoubleSpinBox->setValue(td->getOffset());
          sliceOnlyCheckBox->setChecked(td->isSliceOnly());
      }
      if (volPtr)
      {
          td = getPairCV( volPtr, cathPtr );
          trackGroupBox->setChecked(td->isTracking());
          trackLocSpinBox->setValue(td->getLocation());
          trackOffsetDoubleSpinBox->setValue(td->getOffset());
          axialCheck->setChecked(td->getAxial());
          sagittalCheck->setChecked(td->getSagittal());
          coronalCheck->setChecked(td->getCoronal());
      }
  }
  else
  {
      updateCheckableStatus();
      trackGroupBox->setChecked(false);
      trackLocSpinBox->setValue(0);
      trackOffsetDoubleSpinBox->setValue(1.0); // offset by 1 mm in the camera direction (to aid visibility)
      sliceOnlyCheckBox->setChecked(false);
  }
}

//! Get the object corresponding to a slice, catheter pair.
TrackData* CathTrackingUI::getPairCP(rt2DSliceDataObject* slice, rtCathDataObject* cath) {
  TrackData* res = NULL;

  if (!slice || !cath) return res;

  for (int ix1=0; ix1<m_trackList.count(); ix1++) {
    if ( m_trackList[ix1]->equivalentCPTo(cath, slice) ) {
      res = m_trackList[ix1];
    }
  }

  // Object does not exist yet. Create one!
  if (!res) {
    res = new TrackData(cath, slice);
    m_trackList.append(res);
  }


  return res;
}

//! Get the object corresponding to a volume, catheter pair
TrackData* CathTrackingUI::getPairCV(rt3DVolumeDataObject* vol, rtCathDataObject* cath) {
  TrackData* res = NULL;

  if (!vol || !cath) return res;

  for (int ix1=0; ix1<m_trackList.count(); ix1++) {
    if ( m_trackList[ix1]->equivalentCVTo(cath, vol) ) {
      res = m_trackList[ix1];
    }
  }

  // Object does not exist yet. Create one!
  if (!res) {
    res = new TrackData(cath, vol);
    m_trackList.append(res);
  }


  return res;
}

void CathTrackingUI::updateInfo()
{
    QString infoText;
    QTextStream ts(&infoText, QIODevice::WriteOnly);

    trackedPairs->clear();
    ts << "Tracked pair status: \n";
    ts << "---------------------\n";

    for (int ix1=0; ix1<m_trackList.count(); ix1++)
    {
        ts << "Cath: " << m_trackList[ix1]->getCathObject()->getObjName() << " " << QString::number(m_trackList[ix1]->getCathObject()->getId()) << " , ";
        if (m_trackList[ix1]->getSliceObject())
            ts << "Slice: " << m_trackList[ix1]->getSliceObject()->getObjName() << " " << QString::number(m_trackList[ix1]->getSliceObject()->getId()) ;
        else if (m_trackList[ix1]->getVolObject())
            ts << "Volume: " << m_trackList[ix1]->getVolObject()->getObjName() << " " << QString::number(m_trackList[ix1]->getVolObject()->getId()) ;
        ts << "  Tracked: ";
        if (m_trackList[ix1]->isTracking())
            ts << "ON: ";
        else
            ts << "OFF: ";
        ts << "A:" << m_trackList[ix1]->getAxial() << " S: " << m_trackList[ix1]->getSagittal() << " C: " << m_trackList[ix1]->getCoronal() << "\n\n";
    }
    ts.flush();

    trackedPairs->setPlainText(infoText);
    trackedPairs->setFont(QFont("Courier"));
}


void CathTrackingUI::trackLocationChanged(int loc) {
  int cathId, planeId, volId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  rt3DVolumeDataObject* volPtr = NULL;
  TrackData* td;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();
  volId = m_volumeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  if (volId >= 0)
  {
      volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
  }

  // Not relevant if at least one does not exist.
  if (planePtr && cathPtr) {
    td = getPairCP( planePtr, cathPtr );
    td->setLocation(trackLocSpinBox->value());
    td->update();
  }
  if (volPtr && cathPtr)
  {
      td = getPairCV(volPtr, cathPtr);
      td->setLocation(trackLocSpinBox->value());
      td->update();
  }
  updateInfo();

}


void CathTrackingUI::trackOffsetChanged(double offset) {
    int cathId, planeId, volId;
    rt2DSliceDataObject* planePtr = NULL;
    rtCathDataObject* cathPtr = NULL;
    rt3DVolumeDataObject* volPtr = NULL;
    TrackData* td;

    cathId = m_cathComboBox.getCurrentObjectId();
    planeId = m_planeComboBox.getCurrentObjectId();
    volId = m_volumeComboBox.getCurrentObjectId();

    if (cathId >= 0) {
      cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
    }

    if (planeId >= 0) {
      planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
    }

    if (volId >= 0)
    {
        volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
    }

  // Not relevant if at least one does not exist.
  if (planePtr && cathPtr) {
    td = getPairCP( planePtr, cathPtr );
    td->setOffest( trackOffsetDoubleSpinBox->value() );
    td->update();
  }

  if (volPtr && cathPtr)
  {
      td = getPairCV( volPtr, cathPtr );
      td->setOffest( trackOffsetDoubleSpinBox->value() );
      td->update();
  }
  updateInfo();
}



void CathTrackingUI::trackChanged(bool track) {
    int cathId, planeId, volId;
    rt2DSliceDataObject* planePtr = NULL;
    rtCathDataObject* cathPtr = NULL;
    rt3DVolumeDataObject* volPtr = NULL;
    TrackData* td;

    cathId = m_cathComboBox.getCurrentObjectId();
    planeId = m_planeComboBox.getCurrentObjectId();
    volId = m_volumeComboBox.getCurrentObjectId();

    if (cathId >= 0) {
      cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
    }

    if (planeId >= 0) {
      planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
    }

    if (volId >= 0)
    {
        volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
    }

  // Not relevant if at least one does not exist.
  if (planePtr && cathPtr) {
    td = getPairCP( planePtr, cathPtr );
    if (td) {
        bool ok = true;
        for (int ix1=0; ix1<m_trackList.size(); ix1++)
          if ((m_trackList[ix1]->getCathObject() != td->getCathObject())  && (m_trackList[ix1]->getSliceObject() == td->getSliceObject())  && (m_trackList[ix1]->isTracking()))
              ok = false;
        if (ok)
           td->setTracking(track);
        else
            td->setTrackingOff();
      td->update();
	  
    }
  }

  if (volPtr && cathPtr)
  {
      td = getPairCV( volPtr, cathPtr );
      if (td) {
        bool ok = true;
        for (int ix1=0; ix1<m_trackList.size(); ix1++)
          if ((m_trackList[ix1]->getCathObject() != td->getCathObject())  && (m_trackList[ix1]->getVolObject() == td->getVolObject())  && (m_trackList[ix1]->isTracking()))
              ok = false;
        if (ok)
           td->setTracking(track);
        else
        {
            td->setTrackingOff();
            trackGroupBox->setChecked(false);
        }

        td->update();

      }
  }
  updateInfo();
}

void CathTrackingUI::sliceOnlyChanged(bool value) {
  int cathId, planeId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  TrackData* td = NULL;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  // Not relevant if at least one does not exist.
  if (planePtr && cathPtr) {
    td = getPairCP( planePtr, cathPtr );
    if (td) {
	  td->setSliceOnly(value);
    }
  }
  updateInfo();
}

void CathTrackingUI::planesChanged()
{
    int cathId, volId;
    rtCathDataObject* cathPtr = NULL;
    rt3DVolumeDataObject* volPtr = NULL;
    TrackData* td;

    cathId = m_cathComboBox.getCurrentObjectId();
    volId = m_volumeComboBox.getCurrentObjectId();

    if (cathId >= 0) {
      cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
    }

    if (volId >= 0)
    {
        volPtr = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(volId));
    }

    if (volPtr && cathPtr) {
      td = getPairCV( volPtr, cathPtr );
      if (td) {
            td->setPlanes(axialCheck->isChecked(),sagittalCheck->isChecked(),coronalCheck->isChecked());
      }
    }
    updateInfo();

}
