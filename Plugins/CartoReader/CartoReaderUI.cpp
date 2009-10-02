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
#include <QFileDialog>

#include "rtBaseHandle.h"
#include "CartoFileReader.h"
#include "CartoReaderUI.h"
#include "rt3dPointBufferDataObject.h"

#include <iostream>

CartoReaderUI::CartoReaderUI() {
  setupUi(this);

  m_fName = "";

  connectSignals();


  filePointsTableWidget->setColumnWidth(0, 35);
  filePointsTableWidget->setColumnWidth(1, 60);
  filePointsTableWidget->setColumnWidth(2, 60);
  filePointsTableWidget->setColumnWidth(3, 60);
}

CartoReaderUI::~CartoReaderUI() {
}

void CartoReaderUI::connectSignals() {
  connect(loadFilePushButton, SIGNAL(clicked()), this, SLOT(loadFile()));
  connect(saveAsPointsPushButton, SIGNAL(clicked()), this, SLOT(saveAsPoints()));
  connect(saveAsSurfacePushButton, SIGNAL(clicked()), this, SLOT(saveAsSurface()));
}

void CartoReaderUI::resetTableInfo() {
  QList<CartoFileReader::CartoPoint> pointList;
  pointList = m_customReader.getPointSet();

  // There are no points.
  if (pointList.count() == 0) {
    saveOptionsGroupBox->setEnabled(false);
    saveAsPointsLineEdit->setText("");
    saveAsSurfaceLineEdit->setText("");
    return;
  }

  saveOptionsGroupBox->setEnabled(true);
  saveAsPointsLineEdit->setText(m_customReader.getDataName());
  saveAsSurfaceLineEdit->setText(m_customReader.getDataName());

  filePointsTableWidget->setRowCount(pointList.count());
  for (int ix1=0; ix1<pointList.count(); ix1++) {
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(QString::number(pointList[ix1].id));
    filePointsTableWidget->setItem(ix1, 0, item);

    QTableWidgetItem *itemx = new QTableWidgetItem();
    itemx->setText(QString::number(pointList[ix1].x));
    filePointsTableWidget->setItem(ix1, 1, itemx);

    QTableWidgetItem *itemy = new QTableWidgetItem();
    itemy->setText(QString::number(pointList[ix1].y));
    filePointsTableWidget->setItem(ix1, 2, itemy);

    QTableWidgetItem *itemz = new QTableWidgetItem();
    itemz->setText(QString::number(pointList[ix1].z));
    filePointsTableWidget->setItem(ix1, 3, itemz);
  }
}

void CartoReaderUI::loadFile() {
  m_fName = QFileDialog::getOpenFileName(this, "Open Carto File", "/home", "Carto (*.car)");
  if (m_fName.size() != 0) {
    if ( m_customReader.readFile(m_fName) ) {
      resetTableInfo();
    }
  }
}


void CartoReaderUI::saveAsPoints() {
  QList<CartoFileReader::CartoPoint> pointList;
  pointList = m_customReader.getPointSet();

  if (pointList.count() == 0) return;

  m_pts = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, saveAsPointsLineEdit->text());
  if (m_pts >=0) {
    rt3DPointBufferDataObject* ptObj = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_pts));

    rt3DPointBufferDataObject::SimplePoint sp;
    sp.pSize = 1;

    if (ptObj) {
      ptObj->lock();
      for (int ix1=0; ix1<pointList.count(); ix1++) {
        sp.px = pointList[ix1].x;
        sp.py = pointList[ix1].y;
        sp.pz = pointList[ix1].z;
        ptObj->addPoint(sp);
      }
      ptObj->Modified();
      ptObj->unlock();
    }

    rtBaseHandle::instance().forceRenderUpdate(m_pts);
  }
}

void CartoReaderUI::saveAsSurface() {
  QList<CartoFileReader::CartoPoint> pointList;
  pointList = m_customReader.getPointSet();

  if (pointList.count() == 0) return;


}
