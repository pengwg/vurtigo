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

#include "vtkSurfaceReconstructionFilter.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCell.h"
#include "vtkDelaunay3D.h"
#include "vtkUnstructuredGrid.h"

CartoReaderUI::CartoReaderUI() {
  setupUi(this);

  m_fName = "";

  connectSignals();

  m_pts = -1;
  m_surf[0] = -1;
  m_surf[1] = -1;
  m_surf[2] = -1;

  filePointsTableWidget->setColumnWidth(0, 35);
  filePointsTableWidget->setColumnWidth(1, 60);
  filePointsTableWidget->setColumnWidth(2, 60);
  filePointsTableWidget->setColumnWidth(3, 60);
  filePointsTableWidget->setColumnWidth(9, 45);

  masterTabWidget->setCurrentIndex(0);
  masterTabWidget->setTabEnabled(1, false);
  masterTabWidget->setTabEnabled(2, false);
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
    masterTabWidget->setTabEnabled(1, false);
    masterTabWidget->setTabEnabled(2, false);
    saveAsPointsLineEdit->setText("");
    saveAsSurfaceLineEdit->setText("");
    return;
  }

  masterTabWidget->setTabEnabled(1, true);
  masterTabWidget->setTabEnabled(2, true);
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

    QTableWidgetItem *itemalpha = new QTableWidgetItem();
    itemalpha->setText(QString::number(pointList[ix1].alpha));
    filePointsTableWidget->setItem(ix1, 4, itemalpha);

    QTableWidgetItem *itembeta = new QTableWidgetItem();
    itembeta->setText(QString::number(pointList[ix1].beta));
    filePointsTableWidget->setItem(ix1, 5, itembeta);

    QTableWidgetItem *itemgamma = new QTableWidgetItem();
    itemgamma->setText(QString::number(pointList[ix1].gamma));
    filePointsTableWidget->setItem(ix1, 6, itemgamma);

    QTableWidgetItem *itemuni = new QTableWidgetItem();
    itemuni->setText(QString::number(pointList[ix1].uniPolar));
    filePointsTableWidget->setItem(ix1, 7, itemuni);

    QTableWidgetItem *itembi = new QTableWidgetItem();
    itembi->setText(QString::number(pointList[ix1].biPolar));
    filePointsTableWidget->setItem(ix1, 8, itembi);

    QTableWidgetItem *itemLAT = new QTableWidgetItem();
    itemLAT->setText(QString::number(pointList[ix1].LAT));
    filePointsTableWidget->setItem(ix1, 9, itemLAT);
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
  }
}

void CartoReaderUI::saveAsSurface() {
  QList<CartoFileReader::CartoPoint> pointList;
  QList<rtPolyDataObject::PolyPointLink> linkList;
  bool checkBoxState[3];
  QString SurfType[3];
  rtPolyDataObject* surfObj[3];

  pointList = m_customReader.getPointSet();

  if (pointList.count() == 0) return;

  checkBoxState[0] = uniPolarCheckBox->isChecked();
  checkBoxState[1] = biPolarCheckBox->isChecked();
  checkBoxState[2] = latCheckBox->isChecked();
  SurfType[0] = "UniPolar";
  SurfType[1] = "BiPolar";
  SurfType[2] = "LAT";

  if (!checkBoxState[0] && !checkBoxState[1] && !checkBoxState[2]) return;

  vtkPoints *pts = vtkPoints::New();
  pts->SetNumberOfPoints(pointList.count());

  // Save the points.
  for (int ix1=0; ix1<pointList.count(); ix1++) {
    pts->SetPoint(ix1, pointList[ix1].x, pointList[ix1].y, pointList[ix1].z);
  }
  // Find the polygons
  runVtkDelaunay(pts, linkList);
  pts->Delete();


  for (int ix1=0; ix1<3; ix1++) {
    QString objName;

    objName = saveAsSurfaceLineEdit->text();
    objName.append(SurfType[ix1]);
    m_surf[ix1] = -1;
    if (checkBoxState[ix1]) {
      m_surf[ix1] = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkPolyData, objName);
    }

    surfObj[ix1] = NULL;
    if (m_surf[ix1] >= 0) {
      if (m_surf[ix1] >=0) surfObj[ix1] = static_cast<rtPolyDataObject*>(rtBaseHandle::instance().getObjectWithID(m_surf[ix1]));
    }

    if (surfObj[ix1]) {
      rtPolyDataObject::PolyPoint tempPt;
      QList<rtPolyDataObject::PolyPoint> ptList;

      // Save the point list and the colors
      for (int ix2=0; ix2<pointList.count(); ix2++) {
        tempPt.ptList[0] = pointList[ix2].x;
        tempPt.ptList[1] = pointList[ix2].y;
        tempPt.ptList[2] = pointList[ix2].z;
        if (ix1==0) {
          tempPt.color[0] = (pointList[ix2].uniPolar-m_customReader.getMinUniPolar())/(m_customReader.getMaxUniPolar()-m_customReader.getMinUniPolar())*255.0f;
          tempPt.color[1] = 0;
          tempPt.color[2] = 255-tempPt.color[0];
        } else if (ix1==1) {
          tempPt.color[0] = (pointList[ix2].biPolar-m_customReader.getMinBiPolar())/(m_customReader.getMaxBiPolar()-m_customReader.getMinBiPolar())*255.0f;
          tempPt.color[1] = 0;
          tempPt.color[2] = 255-tempPt.color[0];
        } else if (ix1==2) {
          tempPt.color[0] = ((double)(pointList[ix2].LAT-m_customReader.getMinLAT()))/((double)(m_customReader.getMaxLAT()-m_customReader.getMinLAT()))*255.0f;
          tempPt.color[1] = 0;
          tempPt.color[2] = 255-tempPt.color[0];
        }
        ptList.push_back(tempPt);
      }

      if (m_surf[ix1] >=0 && surfObj[ix1]) {
        surfObj[ix1]->lock();
        surfObj[ix1]->setNewGeometry(&ptList, &linkList);
        surfObj[ix1]->Modified();
        surfObj[ix1]->unlock();
      }
    }
  }
}


bool CartoReaderUI::runVtkDelaunay(vtkPoints* pointsIn, QList<rtPolyDataObject::PolyPointLink> &linkListOut) {
  if (!pointsIn) return false;

  vtkPolyData *pointSet = vtkPolyData::New();
  vtkDelaunay3D* delFilt = vtkDelaunay3D::New();
  rtPolyDataObject::PolyPointLink tempLink;

  pointSet->SetPoints(pointsIn);
  delFilt->SetInput(pointSet);
  delFilt->Update();

  vtkUnstructuredGrid *data = delFilt->GetOutput();
  int numCells = data->GetNumberOfCells();
  for (int ix1=0; ix1<numCells; ix1++) {
    vtkIdList* ptIds = data->GetCell(ix1)->GetPointIds();
    ptIds->GetId(0);

    tempLink.threeVertex[0] = ptIds->GetId(0);
    tempLink.threeVertex[1] = ptIds->GetId(1);
    tempLink.threeVertex[2] = ptIds->GetId(2);
    linkListOut.push_back(tempLink);

    tempLink.threeVertex[0] = ptIds->GetId(0);
    tempLink.threeVertex[1] = ptIds->GetId(1);
    tempLink.threeVertex[2] = ptIds->GetId(3);
    linkListOut.push_back(tempLink);

    tempLink.threeVertex[0] = ptIds->GetId(0);
    tempLink.threeVertex[1] = ptIds->GetId(2);
    tempLink.threeVertex[2] = ptIds->GetId(3);
    linkListOut.push_back(tempLink);

    tempLink.threeVertex[0] = ptIds->GetId(1);
    tempLink.threeVertex[1] = ptIds->GetId(2);
    tempLink.threeVertex[2] = ptIds->GetId(3);
    linkListOut.push_back(tempLink);
  }

  delFilt->Delete();
  pointSet->Delete();
  return true;
}
