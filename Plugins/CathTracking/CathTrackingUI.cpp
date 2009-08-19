#include "CathTrackingUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"


CathTrackingUI::CathTrackingUI() {
  setupUi(this);

  populateLists();
  connectSignals();
}

CathTrackingUI::~CathTrackingUI() {
}

void CathTrackingUI::connectSignals() {
  connect( &(rtObjectManager::instance()), SIGNAL(objectCreated(int)), this, SLOT(objectAdded(int)) );
  connect( &(rtObjectManager::instance()), SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );

  connect( cathComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cathChanged(int)) );
  connect( planeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(planeChanged(int)) );

  connect( trackLocSpinBox, SIGNAL(valueChanged(int)), this, SLOT(trackLocationChanged(int)) );
  connect( trackOffsetDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(trackOffsetChanged(double)) );
}


void CathTrackingUI::populateLists() {
  QList<int> cathObjs, planeObjs;
  rt2DSliceDataObject *slice;
  rtCathDataObject *cath;

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

}

void CathTrackingUI::objectAdded(int objID) {
  //m_planeObjectMap.insert(m_planeObjectMap.count(),
}

void CathTrackingUI::objectRemoved(int objID) {
}


void CathTrackingUI::cathChanged(int) {
}

void CathTrackingUI::planeChanged(int) {
}

void CathTrackingUI::trackLocationChanged(int) {
}

void CathTrackingUI::trackOffsetChanged(double) {
}
