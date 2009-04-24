#include "rtCathDataObject.h"

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

}

//! Clean the GUI widgets.
void rtCathDataObject::cleanupGUI() {

}

//! Return the next unused coil ID
int rtCathDataObject::getNewCoilID() {
  int ix1;
  for (ix1=0; ix1<m_max_coils; ix1++) {
    if (!m_coilList.contains(ix1)) return ix1;
  }
  return -1;
}
