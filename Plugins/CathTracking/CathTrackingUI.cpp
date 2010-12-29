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
#include "rtApplication.h"

#include <QList>

CathTrackingUI::CathTrackingUI() {
  setupUi(this);

  m_cathComboBox.addObjectType(rtConstants::OT_Cath);
  m_planeComboBox.addObjectType(rtConstants::OT_2DObject);

  this->gridLayout->addWidget(&m_cathComboBox, 0, 1, 1, 2);
  this->gridLayout->addWidget(&m_planeComboBox, 1, 1, 1, 2);

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

  connect( trackLocSpinBox, SIGNAL(valueChanged(int)), this, SLOT(trackLocationChanged(int)) );
  connect( trackOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(trackOffsetChanged(double)) );

  connect( trackGroupBox, SIGNAL(toggled(bool)), this, SLOT(trackChanged(bool)));
  
  connect( sliceOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(sliceOnlyChanged(bool)));
}

void CathTrackingUI::updateCheckableStatus() {
  int cathId, planeId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  if ( !cathPtr || !planePtr ) {
    trackGroupBox->setEnabled(false);
    sliceOnlyCheckBox->setEnabled(false);
  }
  else {
    trackGroupBox->setEnabled(true);
    sliceOnlyCheckBox->setEnabled(true);
  }
}

void CathTrackingUI::cathChanged(int newpos) {
  updateCheckableStatus();
  trackingPairChanged();

}

void CathTrackingUI::planeChanged(int newpos) {
  updateCheckableStatus();
  trackingPairChanged();
}


void CathTrackingUI::trackingPairChanged() {
  int cathId, planeId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  TrackData* td;

  cathId = m_cathComboBox.getCurrentObjectId();
  planeId = m_planeComboBox.getCurrentObjectId();

  if (cathId >= 0) {
    cathPtr = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathId));
  }

  if (planeId >= 0) {
    planePtr = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(planeId));
  }

  // Not relevant if at least one does not exist.
  if ( !planePtr || !cathPtr ) {
    updateCheckableStatus();
    trackGroupBox->setChecked(false);
    trackLocSpinBox->setValue(0);
    trackOffsetDoubleSpinBox->setValue(1.0); // offset by 1 mm in the camera direction (to aid visibility)
    sliceOnlyCheckBox->setChecked(false);
  } else {
    td = getPair( planePtr, cathPtr );
    trackGroupBox->setChecked(td->isTracking());
    trackLocSpinBox->setValue(td->getLocation());
    trackOffsetDoubleSpinBox->setValue(td->getOffset());
    sliceOnlyCheckBox->setChecked(td->isSliceOnly());
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
  int cathId, planeId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  TrackData* td;

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
    td = getPair( planePtr, cathPtr );
    td->setLocation(trackLocSpinBox->value());
    td->update();
  }

}


void CathTrackingUI::trackOffsetChanged(double offset) {
  int cathId, planeId;
  rt2DSliceDataObject* planePtr = NULL;
  rtCathDataObject* cathPtr = NULL;
  TrackData* td;

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
    td = getPair( planePtr, cathPtr );
    td->setOffest( trackOffsetDoubleSpinBox->value() );
    td->update();
  }
}



void CathTrackingUI::trackChanged(bool track) {
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
    td = getPair( planePtr, cathPtr );
    if (td) {
      td->setTracking(track);
      td->update();
	  
    }
  }
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
    td = getPair( planePtr, cathPtr );
    if (td) {
	  td->setSliceOnly(value);
    }
  }
}
