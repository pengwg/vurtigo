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
#include "rtPolyDataObject.h"

#include <QHash>
#include <QColor>
#include <QString>

#include <vtkPoints.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>

//! Constructor
rtPolyDataObject::rtPolyDataObject() {
  setObjectType(rtConstants::OT_vtkPolyData);
  setupGUI();

  m_polyData = vtkPolyData::New();
  m_polyProperty = vtkProperty::New();
  m_colorLookup = vtkColorTransferFunction::New();
}

//! Destructor
rtPolyDataObject::~rtPolyDataObject() {
  cleanupGUI();

  if ( m_polyData.GetPointer() ) m_polyData->Delete();
  if ( m_polyProperty.GetPointer() ) m_polyProperty->Delete();
  if ( m_colorLookup.GetPointer() ) m_colorLookup->Delete();
}

//! Set the points and connections for a new poly dataset.
/*!
  This function will remove the old poly dataset and replace it based on the input parameters.
  Long computations are performed in this function so resetting of the geometry should not be done all the time.
  @return True if the replacement was a success.
  */
bool rtPolyDataObject::setNewGeometry(QList<PolyPoint> *pts, QList<PolyPointLink> *links) {
  if (!pts || !links) return false;
  if (pts->empty() || links->empty()) return false;

  vtkPoints *pointList = vtkPoints::New();
  vtkIntArray *scalars = vtkIntArray::New();
  vtkCellArray *surf = vtkCellArray::New();
  int tempLine[3];

  QHash<QString, int> colorTable;
  QColor temp;

  // Remove old table points
  m_colorLookup->RemoveAllPoints();


  pointList->SetNumberOfPoints( pts->count() );
  scalars->SetNumberOfValues( pts->count() );
  for (int ix1=0; ix1<pts->count(); ix1++) {
    pointList->SetPoint(ix1, pts->value(ix1).ptList);
    temp.setRed(pts->value(ix1).color[0]);
    temp.setGreen(pts->value(ix1).color[1]);
    temp.setBlue(pts->value(ix1).color[2]);
    if (!colorTable.contains(temp.name())) {
      colorTable.insert(temp.name(), ix1);
    }
    m_colorLookup->AddRGBPoint(colorTable.value(temp.name()), ((double)temp.red())/255.0f, ((double)temp.green())/255.0f, ((double)temp.blue())/255.0f );
    scalars->SetTupleValue(ix1, &colorTable[temp.name()]);

  }

  m_polyData->Reset();
  m_polyData->SetPoints(pointList);
  m_polyData->GetPointData()->SetScalars(scalars);
  for (int ix1=0; ix1<links->count(); ix1++) {
    tempLine[0] =  (*links)[ix1].threeVertex[0];
    tempLine[1] =  (*links)[ix1].threeVertex[1];
    tempLine[2] =  (*links)[ix1].threeVertex[2];
    surf->InsertNextCell(3, tempLine);
  }

  m_polyData->SetPolys(surf);

  surf->Delete();
  scalars->Delete();
  pointList->Delete();

  return true;
}

//! Copy a new poly data object. The old data is replaced.
bool rtPolyDataObject::copyPolyData(vtkPolyData* polyData) {
  if (!polyData) return false;
  m_polyData->DeepCopy(polyData);
  return true;
}

//! Copy a new lookup table. The old table is replaced.
bool rtPolyDataObject::copyLookupTable(vtkColorTransferFunction* lookupTable) {
  if (!lookupTable) return false;
  m_colorLookup->DeepCopy(lookupTable);
  return true;
}

//! Send the info to the GUI
void rtPolyDataObject::update() {

}

//! Set the GUI widgets.
void rtPolyDataObject::setupGUI() {

}

//! Clean the GUI widgets.
void rtPolyDataObject::cleanupGUI() {

}
