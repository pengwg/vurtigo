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
#include "CathHistoryUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"
#include "rtApplication.h"

#include <QList>

CathHistoryUI::CathHistoryUI() {
  setupUi(this);
  
  m_points = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Catheter History");
  
  m_historyRecorder = NULL;

  markButton->setEnabled(false);
  
  updateCheckableStatus();

  populateLists();
  connectSignals();
}

CathHistoryUI::~CathHistoryUI() {
  if (m_historyRecorder != NULL)
    delete m_historyRecorder;
  m_historyRecorder = NULL;   
    
  if (m_points >= 0)
    {
      // delete it somehow
    }
 
}

void CathHistoryUI::connectSignals() {
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(objectAdded(int)) );
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );

  connect( cathComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cathChanged(int)) );
  
  connect( cathAutoTrackCheckBox, SIGNAL(toggled(bool)), this, SLOT(cathAutoTrackChanged(bool)));

  connect( markButton, SIGNAL(clicked()), this, SLOT(mark()));
}

void CathHistoryUI::updateCheckableStatus() {
  int cathPos;
  cathPos = cathComboBox->currentIndex();
  
  cathAutoTrackCheckBox->setEnabled((cathPos < 0) ? false : true);
}

void CathHistoryUI::populateLists() {
  QList<int> cathObjs;
  QList<int> planeObjs;
  rtCathDataObject* cath=NULL;

  cathObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_Cath);

  cathComboBox->clear();

  for (int ix1=0; ix1<cathObjs.size(); ix1++) {
    cath = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathObjs[ix1]));
    if (cath) {
      m_cathObjectMap.insert(ix1, cath);
      cathComboBox->insertItem(ix1, QString::number(cathObjs[ix1])+cath->getObjName());
    }
  }

  updateCheckableStatus();

}

void CathHistoryUI::objectAdded(int objID) {
  rtDataObject *data=NULL;
  rtCathDataObject *cath=NULL;

  data = rtBaseHandle::instance().getObjectWithID(objID);
  if (!data) return;

  if ( data->getObjectType() == rtConstants::OT_Cath ) {
    cath = static_cast<rtCathDataObject*>(data);
    if (!cath) return;
    m_cathObjectMap.insert(m_cathObjectMap.count(), cath);
    cathComboBox->insertItem(cathComboBox->count(), QString::number(objID)+cath->getObjName());
  }

  updateCheckableStatus();
}

//! The object manager has removed an object. It could be of a type we are interesed in.
void CathHistoryUI::objectRemoved(int objID) {
  int cathPos;

  cathPos = cathComboBox->currentIndex();

  populateLists();

  if ( cathPos >= 0 && cathPos < cathComboBox->count() ) 
    cathComboBox->setCurrentIndex(cathPos);
    
  updateCheckableStatus();
}

void CathHistoryUI::cathChanged(int newpos) {
  trackingPairChanged();
  
  updateCheckableStatus();
}

void CathHistoryUI::trackingPairChanged() {
  int cathPos;

  cathPos = cathComboBox->currentIndex();

  if (cathPos < 0 || m_points < 0) {
   // no selection
    markButton->setEnabled(false);
  } else {
   // valid selection
    markButton->setEnabled(true);
    
    rtCathDataObject *cath = m_cathObjectMap.value(cathPos);
    rt3DPointBufferDataObject *points = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_points));

    if (m_historyRecorder == NULL)
      m_historyRecorder = new HistoryData(cath, points);
    else
      m_historyRecorder->setCathObject(cath);
  }
  
  updateCheckableStatus();
}

void CathHistoryUI::mark() {
  if (m_historyRecorder != NULL)
    m_historyRecorder->savePoint();
}

void CathHistoryUI::cathAutoTrackChanged(bool value) {
  m_historyRecorder->setAutoTrack(value);
}
