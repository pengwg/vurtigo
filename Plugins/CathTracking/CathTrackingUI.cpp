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
#include "CathTrackingUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"

#include <QList>

CathTrackingUI::CathTrackingUI() {
  setupUi(this);

  m_trackList.clear();

  populateLists();
  connectSignals();
}

CathTrackingUI::~CathTrackingUI() {
  TrackData* temp;
  while(m_trackList.count() > 0) {
    temp = m_trackList.takeAt(0);
    delete temp;
  }
}

void CathTrackingUI::connectSignals() {
  connect( &(rtObjectManager::instance()), SIGNAL(objectCreated(int)), this, SLOT(objectAdded(int)) );
  connect( &(rtObjectManager::instance()), SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );

  connect( cathComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cathChanged(int)) );
  connect( planeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(planeChanged(int)) );

  connect( trackLocSpinBox, SIGNAL(valueChanged(int)), this, SLOT(trackLocationChanged(int)) );
  connect( trackOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(trackOffsetChanged(double)) );

  connect( trackGroupBox, SIGNAL(toggled(bool)), this, SLOT(trackChanged(bool)));
}


void CathTrackingUI::populateLists() {
  QList<int> cathObjs;
  QList<int> planeObjs;
  rt2DSliceDataObject* slice=NULL;
  rtCathDataObject* cath=NULL;

  planeObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_2DObject);
  cathObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_Cath);

  cathComboBox->clear();
  planeComboBox->clear();

  for (int ix1=0; ix1<planeObjs.size(); ix1++) {
    slice = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeObjs[ix1]));
    if (slice) {
      m_planeObjectMap.insert(ix1, slice);
      planeComboBox->insertItem(ix1, QString::number(planeObjs[ix1])+slice->getObjName());
    }
  }

  for (int ix1=0; ix1<cathObjs.size(); ix1++) {
    cath = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathObjs[ix1]));
    if (cath) {
      m_cathObjectMap.insert(ix1, cath);
      cathComboBox->insertItem(ix1, QString::number(cathObjs[ix1])+cath->getObjName());
    }
  }
  updateCheckableStatus();
}


void CathTrackingUI::updateCheckableStatus() {
  int cathPos, planePos;
  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();
  if (cathPos < 0 || planePos < 0) {
    trackGroupBox->setEnabled(false);
  }
  else {
    trackGroupBox->setEnabled(true);
  }
}


void CathTrackingUI::objectAdded(int objID) {
  rtDataObject *data=NULL;
  rt2DSliceDataObject *slice=NULL;
  rtCathDataObject *cath=NULL;

  data = rtBaseHandle::instance().getObjectWithID(objID);
  if (!data) return;

  if ( data->getObjectType() == rtConstants::OT_2DObject ) {
    slice = static_cast<rt2DSliceDataObject*>(data);
    if (!slice) return;
    m_planeObjectMap.insert(m_planeObjectMap.count(), slice);
    planeComboBox->insertItem(planeComboBox->count(), QString::number(objID)+slice->getObjName());
  }

  if ( data->getObjectType() == rtConstants::OT_Cath ) {
    cath = static_cast<rtCathDataObject*>(data);
    if (!cath) return;
    m_cathObjectMap.insert(m_cathObjectMap.count(), cath);
    cathComboBox->insertItem(cathComboBox->count(), QString::number(objID)+cath->getObjName());
  }
  updateCheckableStatus();
}

//! The object manager has removed an object. It could be of a type we are interesed in.
void CathTrackingUI::objectRemoved(int objID) {
  int cathPos, planePos;

  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();

  populateLists();

  if ( cathPos >= 0 && cathPos < cathComboBox->count() ) cathComboBox->setCurrentIndex(cathPos);
  if ( planePos >= 0 && planePos < planeComboBox->count() ) planeComboBox->setCurrentIndex(planePos);
  updateCheckableStatus();
}


void CathTrackingUI::cathChanged(int newpos) {
  trackingPairChanged();
}

void CathTrackingUI::planeChanged(int newpos) {
  trackingPairChanged();
}

void CathTrackingUI::trackingPairChanged() {
  int cathPos, planePos;
  TrackData* td;

  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();

  // Not relevant if at least one does not exist.
  if (cathPos < 0 || planePos < 0) {
    updateCheckableStatus();
    trackGroupBox->setChecked(false);
    trackLocSpinBox->setValue(0);
    trackOffsetDoubleSpinBox->setValue(0.0);
  } else {
    td = getPair( m_planeObjectMap.value(planePos), m_cathObjectMap.value(cathPos) );
    trackGroupBox->setChecked(td->isTracking());
    trackLocSpinBox->setValue(td->getLocation());
    trackOffsetDoubleSpinBox->setValue(td->getOffset());
  }
}

//! Get the object corresponding to a slice, catheter pair.
TrackData* CathTrackingUI::getPair(rt2DSliceDataObject* slice, rtCathDataObject* cath) {
  TrackData* res = NULL;

  if (!slice || !cath) return res;

  for (int ix1=0; ix1<m_trackList.count(); ix1++) {
    if ( m_trackList[ix1]->equivalentTo(cath, slice) ) {
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


void CathTrackingUI::trackLocationChanged(int loc) {
  int cathPos, planePos;
  TrackData* td;

  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();

  // Not relevant if at least one does not exist.
  if (cathPos >= 0 && planePos >= 0) {
    td = getPair( m_planeObjectMap.value(planePos), m_cathObjectMap.value(cathPos) );
    td->setLocation(trackLocSpinBox->value());
    if(td->isTracking()) td->update();
  }

}


void CathTrackingUI::trackOffsetChanged(double offset) {
  int cathPos, planePos;
  TrackData* td;

  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();

  // Not relevant if at least one does not exist.
  if (cathPos >= 0 && planePos >= 0) {
    td = getPair( m_planeObjectMap.value(planePos), m_cathObjectMap.value(cathPos) );
    td->setOffest( trackOffsetDoubleSpinBox->value() );
    if(td->isTracking()) td->update();
  }
}


void CathTrackingUI::trackChanged(bool track) {
  int cathPos, planePos;
  TrackData* td;

  cathPos = cathComboBox->currentIndex();
  planePos = planeComboBox->currentIndex();

  // Not relevant if at least one does not exist.
  if (cathPos < 0 || planePos < 0) return;

  td = getPair( m_planeObjectMap.value(planePos), m_cathObjectMap.value(cathPos) );
  if (td) {
    td->setTracking(track);
    if(track) td->update();
  }
}
