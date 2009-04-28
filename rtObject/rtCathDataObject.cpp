#include "rtCathDataObject.h"
#include "rtPropertyChooserDialog.h"

//! Constructor
rtCathDataObject::rtCathDataObject() {
  setObjectType(rtConstants::OT_Cath);
  setupGUI();

  // More than enough coils
  m_max_coils = 128;

  m_eType = ET_MEAN;

  m_splineProperty = vtkProperty::New();
  m_pointProperty = vtkProperty::New();
  m_tipProperty = vtkProperty::New();

}

//! Destructor
rtCathDataObject::~rtCathDataObject() {
  cleanupGUI();

  m_splineProperty->Delete();
  m_pointProperty->Delete();
  m_tipProperty->Delete();
}


//! Take the info from the GUI
void rtCathDataObject::apply() {

}

//! Send the info to the GUI
void rtCathDataObject::update() {

}

// Coil specific
//! Add a coil to the list at a partiucular location.
/*!
  Each coil must have a location before it is created. Multiple coils can be at the same location.
  @param loc The location of the coil. Multiple coils can be at the same location.
  @return The coil ID for the new coil or -1 if the coil could not be added.
  */
int rtCathDataObject::addCoil(int loc) {
  int cID;
  rtCathDataObject::CathCoilData coil;

  cID = getNewCoilID();
  if (cID >= 0) {
    coil.locationID = loc;
    coil.cx = 0.0;
    coil.cy = 0.0;
    coil.cz = 0.0;
    coil.SNR = 1;
    m_coilList.insert(cID, coil);

    m_coilLocations.insert(loc, cID);
  }

  return cID;
}

//! Set the SNR for a coil
bool rtCathDataObject::setCoilSNR(int coilID, int SNR) {
  if (m_coilList.contains(coilID)) {
    m_coilList[coilID].SNR = SNR;
    return true;
  }
  return false;
}

//! Set the angles for a coil.
bool rtCathDataObject::setCoilAngles(int coilID, int a1, int a2) {
  if (m_coilList.contains(coilID)) {
    m_coilList[coilID].angles[0] = a1;
    m_coilList[coilID].angles[1] = a2;
    return true;
  }
  return false;
}

//! Set the coords for a coil.
bool rtCathDataObject::setCoilCoords(int coilID, double cx, double cy, double cz) {
  if (m_coilList.contains(coilID)) {
    m_coilList[coilID].cx = cx;
    m_coilList[coilID].cy = cy;
    m_coilList[coilID].cz = cz;
    return true;
  }
  return false;
}

//! Get a handle for a particular coil object.
rtCathDataObject::CathCoilData* rtCathDataObject::getCoilHandle(int coilID) {
  if (m_coilList.contains(coilID)) {
    return &m_coilList[coilID];
  }
  return NULL;
}

//! Remove a coil from the list.
bool rtCathDataObject::removeCoil(int coilID) {
  if (m_coilList.contains(coilID)){
    m_coilLocations.remove(m_coilList[coilID].locationID, coilID);
    m_coilList.remove(coilID);
    return true;
  }
  return false;
}

// List specific
//! Get the number of coils in the list.
int rtCathDataObject::getNumCoils() {
  return m_coilList.size();
}

//! Get the handle to the coil list.
/*!
  The list should not be modified by the caller.
  @returnTthe handle to the coil list.
  */
QHash<int, rtCathDataObject::CathCoilData>* rtCathDataObject::getListHandle() {
  return &m_coilList;
}

//! The the number of unique locations.
int rtCathDataObject::getNumLocations() {
  return m_coilLocations.uniqueKeys().size();
}

//! List of unique locations on the catheter.
QList<int> rtCathDataObject::getLocationList() {
  return m_coilLocations.uniqueKeys();
}

//! Get the calculated position at a certain location
/*!
  Locations may have multiple coils and the position must be estimated.
  @param loc The location ID
  @param out Resulting position (x,y,z) will be placed here.
  @return True if the resulting position is valid. False otherwise.
  */
bool rtCathDataObject::getPositionAtLocation(int loc, double out[3]) {
  if(!m_coilLocations.contains(loc)) return false;

  QList<int> Ids = m_coilLocations.values(loc);
  int ix1=0;
  double sumSNR=0;
  double maxSNR=0;

  out[0] = 0; out[1] = 0; out[2] = 0;

  switch(m_eType) {
    case (ET_MEAN):
    for (ix1=0; ix1<Ids.size(); ix1++) {
      out[0] = out[0] + m_coilList[Ids[ix1]].cx;
      out[1] = out[1] + m_coilList[Ids[ix1]].cy;
      out[2] = out[2] + m_coilList[Ids[ix1]].cz;
    }
    // Then divide by the number of coils.
    out[0] = out[0] / (double)Ids.size();
    out[1] = out[1] / (double)Ids.size();
    out[2] = out[2] / (double)Ids.size();

    break;
    case (ET_WTMEAN):
    // Weighted mean based on SNR
    for (ix1=0; ix1<Ids.size(); ix1++) {
      sumSNR = sumSNR + m_coilList[Ids[ix1]].SNR;
      out[0] = out[0] + m_coilList[Ids[ix1]].cx*m_coilList[Ids[ix1]].SNR;
      out[1] = out[1] + m_coilList[Ids[ix1]].cy*m_coilList[Ids[ix1]].SNR;
      out[2] = out[2] + m_coilList[Ids[ix1]].cz*m_coilList[Ids[ix1]].SNR;
    }
    // Then divide by the number of coils.
    out[0] = out[0] / sumSNR;
    out[1] = out[1] / sumSNR;
    out[2] = out[2] / sumSNR;
    break;
    case(ET_BESTSNR):
    for (ix1=0; ix1<Ids.size(); ix1++) {
      if ( m_coilList[Ids[ix1]].SNR > maxSNR ) {
        out[0] = m_coilList[Ids[ix1]].cx;
        out[1] = m_coilList[Ids[ix1]].cy;
        out[2] = m_coilList[Ids[ix1]].cz;
        maxSNR = m_coilList[Ids[ix1]].SNR;
      }
    }
    break;
  }

  return true;
}

//! Get the SNR at a particular location
/*!
  The SNR at a location is also defined by the estimation method.
  */
bool rtCathDataObject::getSNRAtLocation(int loc, double &SNR) {
  if(!m_coilLocations.contains(loc)) return false;

  QList<int> Ids = m_coilLocations.values(loc);
  int ix1;
  double sumSNR;
  SNR = 0;

  switch(m_eType) {
    case (ET_MEAN):
    for (ix1=0; ix1<Ids.size(); ix1++) {
      SNR  = SNR + m_coilList[Ids[ix1]].SNR;
    }
    // Then divide by the number of coils.
    SNR = SNR / (double)Ids.size();

    break;
    case (ET_WTMEAN):
    // Weighted mean based on SNR
    for (ix1=0; ix1<Ids.size(); ix1++) {
      sumSNR = sumSNR + m_coilList[Ids[ix1]].SNR;
      SNR  = SNR + m_coilList[Ids[ix1]].SNR*m_coilList[Ids[ix1]].SNR;
    }
    SNR = SNR / sumSNR ;
    break;
    case(ET_BESTSNR):
    for (ix1=0; ix1<Ids.size(); ix1++) {
      if ( m_coilList[Ids[ix1]].SNR > SNR ) {
        SNR = m_coilList[Ids[ix1]].SNR;
      }
    }
    break;
  }

  return true;
}


//! Set the GUI widgets.
void rtCathDataObject::setupGUI() {
  QWidget* wid = getBaseWidget();

  m_masterLayout = new QGridLayout();
  wid->setLayout(m_masterLayout);

  m_splinePropLabel.setText("Spline Options: ");
  m_splinePropButton.setText("Property");
  m_masterLayout->addWidget(&m_splinePropLabel, 0, 0);
  m_masterLayout->addWidget(&m_splinePropButton, 0, 1);
  connect(&m_splinePropButton, SIGNAL(pressed()), this, SLOT(splinePropertyDialog()));

  m_pointPropLabel.setText("Sphere Options: ");
  m_pointPropButton.setText("Property");
  m_masterLayout->addWidget(&m_pointPropLabel, 1, 0);
  m_masterLayout->addWidget(&m_pointPropButton, 1, 1);
  connect(&m_pointPropButton, SIGNAL(pressed()), this, SLOT(pointPropertyDialog()));

  m_tipPropLabel.setText("Cath Tip Options: ");
  m_tipPropButton.setText("Property");
  m_masterLayout->addWidget(&m_tipPropLabel, 2, 0);
  m_masterLayout->addWidget(&m_tipPropButton, 2, 1);
  connect(&m_tipPropButton, SIGNAL(pressed()), this, SLOT(tipPropertyDialog()));
}

//! Clean the GUI widgets.
void rtCathDataObject::cleanupGUI() {
  if (m_masterLayout) delete m_masterLayout;
}

//! Return the next unused coil ID
int rtCathDataObject::getNewCoilID() {
  int ix1;
  for (ix1=0; ix1<m_max_coils; ix1++) {
    if (!m_coilList.contains(ix1)) return ix1;
  }
  return -1;
}

//! Called when the spline properties are requested.
void rtCathDataObject::splinePropertyDialog() {
  rtPropertyChooserDialog splineDlg(m_splineProperty);
  splineDlg.exec();
  if (splineDlg.isChanged()) Modified();
}

//! Called when the point properties are requested.
void rtCathDataObject::pointPropertyDialog() {
  rtPropertyChooserDialog pointDlg(m_pointProperty);
  pointDlg.exec();
  if (pointDlg.isChanged()) Modified();
}


//! Called when the tip properties are requested.
void rtCathDataObject::tipPropertyDialog() {
  rtPropertyChooserDialog tipDlg(m_tipProperty);
  tipDlg.exec();
  if (tipDlg.isChanged()) Modified();
}

