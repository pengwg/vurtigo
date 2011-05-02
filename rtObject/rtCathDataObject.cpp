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
#include "rtCathDataObject.h"


//! Constructor
rtCathDataObject::rtCathDataObject()
 : m_tension(0.0), m_continuity(0.0), m_tipValue(0.0), m_endValue(0.0), m_splineThickness(3.0)
{
  setObjectType("OT_Cath");

  // Allocate new objects
  m_splineProperty = vtkProperty::New();
  m_pointProperty = vtkProperty::New();
  m_tipProperty = vtkProperty::New();

  m_splinePropertyDlg = new rtPropertyChooserDialog(m_splineProperty);
  m_pointPropertyDlg = new rtPropertyChooserDialog(m_pointProperty);
  m_tipPropertyDlg = new rtPropertyChooserDialog(m_tipProperty);

  // Build the GUI
  setupGUI();

  // More than enough coils
  m_max_coils = 128;


  // Set the default point size.
  m_pointSize = 10;
  m_cathGuiSetup.pointSizeSlider->setValue(m_pointSize);
  pointSizeChanged(m_pointSize);

  m_eType = ET_MEAN;

  // Start with no coils...
  m_coilIDList.clear();

  connect(this, SIGNAL(updateCoilTableSignal()), this, SLOT(updateCoilTable()), Qt::QueuedConnection);
}

//! Destructor
rtCathDataObject::~rtCathDataObject() {
  cleanupGUI();

  m_splineProperty->Delete();
  m_pointProperty->Delete();
  m_tipProperty->Delete();

  delete m_splinePropertyDlg;
  delete m_pointPropertyDlg;
  delete m_tipPropertyDlg;
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
    coil.visible = true;
    m_coilList.insert(cID, coil);
    m_coilLocations.insert(loc, cID);
    m_coilIDList.append(cID);
    emit updateCoilTableSignal();
  }

  return cID;
}

//! Set the SNR for a coil
bool rtCathDataObject::setCoilSNR(int coilID, double SNR) {
  if (SNR <= 0.01) SNR = 0.01;

  if(!m_coilIDList.contains(coilID)) return false;

  if (m_coilList.contains(coilID)) {  
    m_coilList[coilID].SNR = SNR;
    emit updateCoilTableSignal();
    return true;
  } else if (m_discardCoilList.contains(coilID)) {
    m_discardCoilList[coilID].SNR = SNR;
    emit updateCoilTableSignal();
    return true;
  }
  return false;
}

//! Set the angles for a coil.
bool rtCathDataObject::setCoilAngles(int coilID, int a1, int a2) {
  if(!m_coilIDList.contains(coilID)) return false;

  if (m_coilList.contains(coilID)) {
    m_coilList[coilID].angles[0] = a1;
    m_coilList[coilID].angles[1] = a2;
    emit updateCoilTableSignal();
    return true;
  } else if (m_discardCoilList.contains(coilID)) {
    m_discardCoilList[coilID].angles[0] = a1;
    m_discardCoilList[coilID].angles[1] = a2;
    emit updateCoilTableSignal();
    return true;
  }
  return false;
}

//! Set the coords for a coil.
bool rtCathDataObject::setCoilCoords(int coilID, double cx, double cy, double cz) {
  if(!m_coilIDList.contains(coilID)) return false;

  if (m_coilList.contains(coilID)) {
    m_coilList[coilID].cx = cx;
    m_coilList[coilID].cy = cy;
    m_coilList[coilID].cz = cz;
    emit updateCoilTableSignal();
    return true;
  } else if (m_discardCoilList.contains(coilID)) {
    m_discardCoilList[coilID].cx = cx;
    m_discardCoilList[coilID].cy = cy;
    m_discardCoilList[coilID].cz = cz;
    emit updateCoilTableSignal();
    return true;
  }
  return false;
}

//! Get a handle for a particular coil object.
rtCathDataObject::CathCoilData* rtCathDataObject::getCoilHandle(int coilID) {  
  if (m_coilList.contains(coilID)) {
    return &m_coilList[coilID];
  } else if (m_discardCoilList.contains(coilID)) {
    return &m_discardCoilList[coilID];
  }
  return NULL;
}

//! Remove a coil from the list.
bool rtCathDataObject::removeCoil(int coilID) {
  if (m_coilList.contains(coilID)){
    m_coilLocations.remove(m_coilList[coilID].locationID, coilID);
    m_coilList.remove(coilID);
    m_coilIDList.removeAll(coilID);
    emit updateCoilTableSignal();
    return true;
  } else if (m_discardCoilList.contains(coilID)) {
    m_discardCoilList.remove(coilID);
    m_coilIDList.removeAll(coilID);
    emit updateCoilTableSignal();
    return true;
  }
  return false;
}

// List specific
//! Get the number of visible coils in the list.
/*!
  \return The number of visible coils in the coil list.
  */
int rtCathDataObject::getNumCoils() {
  return m_coilList.size();
}

//! Get the number of coils in the list.
/*!
  \return The number of coils in the coil list. This includes coils that are not visible.
  */
int rtCathDataObject::getNumAllCoils() {
  return m_coilIDList.size();
}

//! Get the handle to the coil list.
/*!
  The list should not be modified by the caller. This includes ONLY VISIBLE coils.
  \return The handle to the coil list.
  */
QHash<int, rtCathDataObject::CathCoilData>* rtCathDataObject::getListHandle() {
  return &m_coilList;
}

//! The the number of unique visible locations.
int rtCathDataObject::getNumLocations() {
  return m_coilLocations.uniqueKeys().size();
}

//! List of unique visible locations on the catheter.
QList<int> rtCathDataObject::getLocationList() {
  QList<int> Ids = m_coilLocations.uniqueKeys();
  return Ids;
}

//! Get the calculated position at a certain location
/*!
  Locations may have multiple coils and the position must be estimated. Coils that are not visible are not included in the calculation.
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
  The SNR at a location is also defined by the estimation method. Coils that are not visible are not included in the calculation.
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

  if (SNR < 0.01) {
    // Safety test. All SNR of less than 0.01 is defaulted to the minimum.
    SNR = 0.01;
  }

  return true;
}


//! Set the GUI widgets.
void rtCathDataObject::setupGUI() {
  QWidget* wid = getBaseWidget();

  m_cathGuiSetup.setupUi(wid);

  // setup the use SNR checkbox.
  m_useSNRSize = false;
  m_cathGuiSetup.snrSizeCheckBox->setChecked(m_useSNRSize);

  // Setup the points table.
  m_cathGuiSetup.pointsTable->setColumnWidth(0, 60);
  m_cathGuiSetup.pointsTable->setColumnWidth(1, 50);
  m_cathGuiSetup.pointsTable->setColumnWidth(2, 60);
  m_cathGuiSetup.pointsTable->setColumnWidth(3, 50);
  m_cathGuiSetup.pointsTable->setColumnWidth(4, 50);
  m_cathGuiSetup.pointsTable->setColumnWidth(5, 50);
  m_cathGuiSetup.pointsTable->setColumnWidth(6, 50);

  connect(m_cathGuiSetup.pointsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableCellChanged(int,int)));

  connect(m_cathGuiSetup.splinePropButton, SIGNAL(pressed()), this, SLOT(splinePropertyDialog()));
  connect(m_cathGuiSetup.pointPropButton, SIGNAL(pressed()), this, SLOT(pointPropertyDialog()));
  connect(m_cathGuiSetup.tipPropButton, SIGNAL(pressed()), this, SLOT(tipPropertyDialog()));

  connect(m_cathGuiSetup.pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(pointSizeChanged(int)));

  connect(m_splinePropertyDlg, SIGNAL(propertyChanged()), this, SLOT(Modified()));
  connect(m_pointPropertyDlg, SIGNAL(propertyChanged()), this, SLOT(Modified()));
  connect(m_tipPropertyDlg, SIGNAL(propertyChanged()), this, SLOT(Modified()));

  connect(m_cathGuiSetup.snrSizeCheckBox, SIGNAL(stateChanged(int)),  this, SLOT(useSNRSizeChanged(int)));

  connect( m_cathGuiSetup.thicknessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(splineThicknessChanged(double)) );
  connect( m_cathGuiSetup.tensionSpinBox, SIGNAL(valueChanged(double)), this, SLOT(tensionChanged(double)) );
  connect( m_cathGuiSetup.continuitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(continuityChanged(double)) );
  connect( m_cathGuiSetup.tipSpinBox, SIGNAL(valueChanged(double)), this, SLOT(tipValueChanged(double)) );
  connect( m_cathGuiSetup.endSpinBox, SIGNAL(valueChanged(double)), this, SLOT(endValueChanged(double)) );
}

//! Clean the GUI widgets.
void rtCathDataObject::cleanupGUI() {
}

//! Called when the spline properties are requested.
void rtCathDataObject::splinePropertyDialog() {
  m_splinePropertyDlg->show();
}

//! Called when the point properties are requested.
void rtCathDataObject::pointPropertyDialog() {
  m_pointPropertyDlg->show();
}


//! Called when the tip properties are requested.
void rtCathDataObject::tipPropertyDialog() {
  m_tipPropertyDlg->show();
}

//! Called when the slider changes and the point sizes are changed.
void rtCathDataObject::pointSizeChanged(int size) {
  m_cathGuiSetup.pointSizeLabel->setText(QString::number(size) + " X.");
  m_pointSize = size;
  Modified();
}

//! The SNR check box was changed
void rtCathDataObject::useSNRSizeChanged(int status) {
  if (status == Qt::Unchecked) {
    m_useSNRSize = false;
  } else {
    m_useSNRSize = true;
  }
  Modified();
}


void rtCathDataObject::tableCellChanged(int row, int col) {
  // Is it one of the check boxes?
  if(col == 0) {
    QTableWidgetItem* item = m_cathGuiSetup.pointsTable->item(row, col);
    QTableWidgetItem* itemID = m_cathGuiSetup.pointsTable->item(row, col+1);

    if (!item || !itemID) return;

    int id=0;
    bool ok=false;

    id = itemID->text().toInt(&ok);

    if ( ok ) {
      if (item->checkState()==Qt::Checked) {
        item->setText("ON");
        item->setCheckState(Qt::Checked);
        // Get from the discard pile...
        if(m_discardCoilList.contains(id)) {
          m_coilList.insert(id, m_discardCoilList.take(id));
          m_coilList[id].visible = true;
          m_coilLocations.insert(m_coilList[id].locationID, id);
        }

      } else if (item->checkState()==Qt::Unchecked) {
        item->setText("OFF");
        item->setCheckState(Qt::Unchecked);
        if (m_coilList.contains(id)) {
          // Send to discard pile...
          m_coilList[id].visible = false;
          m_coilLocations.remove(m_coilList[id].locationID, id);
          m_discardCoilList.insert(id, m_coilList.take(id));
        }
      }

      Modified();
    }
  }
}

//! Return the next unused coil ID
int rtCathDataObject::getNewCoilID() {
  int ix1;
  for (ix1=0; ix1<m_max_coils; ix1++) {
    if (!m_coilList.contains(ix1) && !m_discardCoilList.contains(ix1)) return ix1;
  }
  return -1;
}

void rtCathDataObject::updateCoilTable() {
  QTableWidgetItem *tempItem;
  CathCoilData dat;

  // Set the number of rows if required.
  if( m_cathGuiSetup.pointsTable->rowCount() != m_coilIDList.size() ) {
    m_cathGuiSetup.pointsTable->setRowCount(m_coilIDList.size());
  }

  for (int ix1=0; ix1<m_coilIDList.size(); ix1++) {
    // Get the coil data.
    if( m_coilList.contains(m_coilIDList[ix1]) ) {
      dat = m_coilList[m_coilIDList[ix1]];
    } else if ( m_discardCoilList.contains(m_coilIDList[ix1]) ) {
      dat = m_discardCoilList[m_coilIDList[ix1]];
    }

    tempItem = m_cathGuiSetup.pointsTable->item(ix1,0);
    if( !tempItem ) {
      tempItem = new QTableWidgetItem();
      tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    }

    if ( dat.visible && tempItem->checkState()!=Qt::Checked ) {
      tempItem->setCheckState(Qt::Checked);
      tempItem->setText("ON");
    } else if ( !dat.visible && tempItem->checkState()!=Qt::Unchecked ) {
      tempItem->setCheckState(Qt::Unchecked);
      tempItem->setText("OFF");
    }
    if ( !m_cathGuiSetup.pointsTable->item(ix1,0) ) m_cathGuiSetup.pointsTable->setItem(ix1, 0, tempItem);

    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(m_coilIDList[ix1]));
    m_cathGuiSetup.pointsTable->setItem(ix1, 1, tempItem);

    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(dat.locationID));
    m_cathGuiSetup.pointsTable->setItem(ix1, 2, tempItem);

    // X
    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(dat.cx));
    m_cathGuiSetup.pointsTable->setItem(ix1, 3, tempItem);

    // Y
    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(dat.cy));
    m_cathGuiSetup.pointsTable->setItem(ix1, 4, tempItem);

    // Z
    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(dat.cz));
    m_cathGuiSetup.pointsTable->setItem(ix1, 5, tempItem);

    // SNR
    tempItem = new QTableWidgetItem();
    tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tempItem->setText(QString::number(dat.SNR));
    m_cathGuiSetup.pointsTable->setItem(ix1, 6, tempItem);
  }
}


void rtCathDataObject::splineThicknessChanged(double thickness) {
  m_splineThickness = thickness;
  Modified();
}

void rtCathDataObject::tensionChanged(double tension) {
  m_tension = tension;
  Modified();
}

void rtCathDataObject::continuityChanged(double continuity) {
  m_continuity = continuity;
  Modified();
}

void rtCathDataObject::tipValueChanged(double tip) {
  m_tipValue = tip;
  Modified();
}

void rtCathDataObject::endValueChanged(double end) {
  m_endValue = end;
  Modified();
}
