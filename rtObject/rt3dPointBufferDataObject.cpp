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
#include <climits>

#include "rt3dPointBufferDataObject.h"

//! Constructor
rt3DPointBufferDataObject::rt3DPointBufferDataObject()
: m_currentScale(1.0)
{
  m_pointList.clear();
  setObjectType(rtConstants::OT_3DPointBuffer);
  setupGUI();
}

//! Destructor
rt3DPointBufferDataObject::~rt3DPointBufferDataObject() {
  m_pointList.clear();
  cleanupGUI();
}

rtBasic3DPointData* rt3DPointBufferDataObject::getPointAt(double x, double y, double z) {
  rtBasic3DPointData* res = 0;
  for (int ix1=0; ix1<m_pointList.size(); ix1++) {
    if (m_pointList[ix1].getX() == x && m_pointList[ix1].getY() == y && m_pointList[ix1].getZ() == z) {
      res = &(m_pointList[ix1]);
    }
  }
  return res;
}

rtBasic3DPointData* rt3DPointBufferDataObject::getPointWithId(int id) {
  rtBasic3DPointData* res = 0;
  for (int ix1=0; ix1<m_pointList.size(); ix1++) {
    if (m_pointList[ix1].getPointId() == id) {
      res = &(m_pointList[ix1]);
    }
  }
  return res;
}

rtBasic3DPointData* rt3DPointBufferDataObject::getPointAtIndex(int index) {
   if ((index < 0) || (index >= m_pointList.size()))
      return NULL;
      
    return &m_pointList[index];
}


//! Add a point to the list
void rt3DPointBufferDataObject::addPoint(rtBasic3DPointData sp) {
  rtNamedInfoPointData namedPt;
  int id = getNextId();

  if (id != -1) {
    sp.setPointId(id);
    m_pointList.append(sp);
    namedPt.fromBasic3DPoint(&sp);
    m_namedInfoData.insert(id, namedPt);
    Modified();
    updateGuiPointList();
  }
}

//! Remove a point from the list
void rt3DPointBufferDataObject::removePoint(rtBasic3DPointData sp) {
  int i;

  if (m_pointList.contains(sp)) {
    i = m_pointList.indexOf(sp);
    m_namedInfoData.remove(sp.getPointId());
    m_pointList.removeAt(i);
    Modified();
    updateGuiPointList();
  }
}

void rt3DPointBufferDataObject::addCartoPoint(rtCartoPointData pt) {
  rtNamedInfoPointData namedPt;
  int id = getNextId();

  if (id != -1) {
    pt.setPointId(id);
    // Append to the regular point list.
    m_pointList.append(pt);
    namedPt.fromCartoPoint(&pt);
    m_namedInfoData.insert(id, namedPt);
    Modified();
    updateGuiPointList();
  }
}


//! Send the info to the GUI
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::update() {

}

void rt3DPointBufferDataObject::applyTransformToPoints(vtkTransform * t) {
  if (!t) return;

  for (int ix1=0; ix1<m_pointList.size(); ix1++) {
    m_pointList[ix1].applyTransform(t);
    m_namedInfoData[m_pointList[ix1].getPointId()].applyTransform(t);
  }
  Modified();
  updateGuiPointList();
}

void rt3DPointBufferDataObject::applyTranslateToPoints(double x, double y, double z) {
  for (int ix1=0; ix1<m_pointList.size(); ix1++) {
    m_pointList[ix1].translate(x,y,z);
    m_namedInfoData[m_pointList[ix1].getPointId()].translate(x,y,z);
  }
  Modified();
  updateGuiPointList();
}


/////////////
// Public slots
/////////////


void rt3DPointBufferDataObject::transPlusX() {
  applyTranslateToPoints(1.0, 0.0, 0.0);
  Modified();
}

void rt3DPointBufferDataObject::transMinusX() {
  applyTranslateToPoints(-1.0, 0.0, 0.0);
  Modified();
}

void rt3DPointBufferDataObject::transPlusY() {
  applyTranslateToPoints(0.0, 1.0, 0.0);
  Modified();
}

void rt3DPointBufferDataObject::transMinusY() {
  applyTranslateToPoints(0.0, -1.0, 0.0);
  Modified();
}

void rt3DPointBufferDataObject::transPlusZ() {
  applyTranslateToPoints(0.0, 0.0, 1.0);
  Modified();
}

void rt3DPointBufferDataObject::transMinusZ() {
  applyTranslateToPoints(0.0, 0.0, -1.0);
  Modified();
}

void rt3DPointBufferDataObject::rotPlusX() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateX(5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusX() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateX(-5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotPlusY() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateY(5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusY() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateY(-5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotPlusZ() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateZ(5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusZ() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateZ(-5.0);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::scaleChanged(double val) {
  double scaleChange = val / m_currentScale;
  m_currentScale = val;

  vtkTransform * temp= vtkTransform::New();
  temp->Scale(scaleChange, scaleChange, scaleChange);
  applyTransformToPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::addNewTagButton() {
  QString tagName = m_optionsWidget.addPropertyLineEdit->text();
  tagName = tagName.trimmed();

  if (tagName != "") {
    if (!m_columnHeaderList.contains(tagName)) {
      m_columnHeaderList.append(tagName);
      Modified();
      updateGuiPointList();
    }
  }
}

void rt3DPointBufferDataObject::updateGuiPointList() {
  QHash<int, rtNamedInfoPointData>::iterator i;
  QList<QString> tagList;
  int index=0;

  // Disconnect the table so that we don't get all the changed events.
  disconnect( m_optionsWidget.pointsTable, SIGNAL( cellChanged(int,int) ), this, SLOT( tableCellChanged(int,int) ) );

  // Update the list of custom column headers.
  updateColumnHeaders();

  m_optionsWidget.pointsTable->clear();

  m_optionsWidget.pointsTable->setColumnCount(5+m_columnHeaderList.size());

  m_optionsWidget.pointsTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Point ID") );
  m_optionsWidget.pointsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("X") );
  m_optionsWidget.pointsTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Y") );
  m_optionsWidget.pointsTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Z") );
  m_optionsWidget.pointsTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Timestamp (ms)") );

  // Add the custom headers (if any)
  for (int ix1=0; ix1<m_columnHeaderList.size(); ix1++) {
    m_optionsWidget.pointsTable->setHorizontalHeaderItem(5+ix1, new QTableWidgetItem(m_columnHeaderList[ix1]) );
  }

  m_optionsWidget.pointsTable->setRowCount(m_namedInfoData.size());
  index = 0;
  for (i = m_namedInfoData.begin(); i != m_namedInfoData.end(); ++i) {
    m_optionsWidget.pointsTable->setItem( index, 0, new QTableWidgetItem(QString::number(i.key())) );
    m_optionsWidget.pointsTable->item( index, 0 )->setFlags(Qt::ItemIsSelectable);
    m_optionsWidget.pointsTable->setItem( index, 1, new QTableWidgetItem(QString::number(i.value().getX())) );
    m_optionsWidget.pointsTable->setItem( index, 2, new QTableWidgetItem(QString::number(i.value().getY())) );
    m_optionsWidget.pointsTable->setItem( index, 3, new QTableWidgetItem(QString::number(i.value().getZ())) );
    m_optionsWidget.pointsTable->setItem( index, 4, new QTableWidgetItem(QString::number(i.value().getCreationTime())) );
    m_optionsWidget.pointsTable->item( index, 4 )->setFlags(Qt::ItemIsSelectable);

    // Do the custom headings as well.
    for (int ix1=0; ix1<m_columnHeaderList.size(); ix1++) {
      if (i.value().tagExists(m_columnHeaderList[ix1])) {
        m_optionsWidget.pointsTable->setItem( index, 5+ix1, new QTableWidgetItem(QString::number( i.value().getValue(m_columnHeaderList[ix1]) )) );
      }
    }

    index++;
  }

  // Connect the table again at the end.
  connect( m_optionsWidget.pointsTable, SIGNAL( cellChanged(int,int) ), this, SLOT( tableCellChanged(int,int) ) );

}

void rt3DPointBufferDataObject::tableSelectionChanged() {
  QList<QTableWidgetItem *> selected;

  selected = m_optionsWidget.pointsTable->selectedItems();
  m_selectedItems.clear();
  for (int ix1=0; ix1<selected.size(); ix1++) {
    if (selected[ix1]->column() == 0) {
      m_selectedItems.append( selected[ix1]->text().toInt() );
    }
  }
  Modified();
}

void rt3DPointBufferDataObject::tableCellChanged(int row, int col) {
  // Make sure that the IDs and timestamps are not being changed.
  if (col == 0 || col == 4) return;

  QTableWidgetItem* item;
  QTableWidgetItem* itemId;
  int id;
  double newValue;
  bool ok;
  rtBasic3DPointData* pointHandle;

  item = m_optionsWidget.pointsTable->item(row, col);
  itemId = m_optionsWidget.pointsTable->item(row, 0);
  id = itemId->text().toInt(&ok);

  if (ok && m_namedInfoData.contains(id)) {
    newValue = item->text().toDouble(&ok);
    pointHandle = getPointWithId(id);

    if (ok) {
      // Value is a valid double. Must update data structures.
      if (col == 1) {
        m_namedInfoData[id].setX(newValue);
        pointHandle->setX(newValue);
      } else if (col == 2) {
        m_namedInfoData[id].setY(newValue);
        pointHandle->setY(newValue);
      } else if (col == 3) {
        m_namedInfoData[id].setZ(newValue);
        pointHandle->setZ(newValue);
      } else {
        // A custom tag.
        m_namedInfoData[id].setNamedValue(m_columnHeaderList[col-5], newValue);
      }

      // Both numbers are OK so we can call modified.
      Modified();

    } else {
      // Value is not a valid double.
      // Must replace string with the old value.
      if (col == 1) {
        item->setText(QString::number(m_namedInfoData[id].getX()));
      } else if (col == 2) {
        item->setText(QString::number(m_namedInfoData[id].getY()));
      } else if (col == 3) {
        item->setText(QString::number(m_namedInfoData[id].getZ()));
      } else {
        // A custom tag.
        if (m_namedInfoData[id].tagExists(m_columnHeaderList[col-5])) {
          item->setText(QString::number(m_namedInfoData[id].getValue(m_columnHeaderList[col-5])));
        } else {
          item->setText("");
        }
      }

    }
  }
}

////////////
// Protected
////////////

void rt3DPointBufferDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());


  // Buttons above the points table

  connect( m_optionsWidget.addPropertyPushButton, SIGNAL(clicked()), this, SLOT(addNewTagButton()) );

  // Setup the points table

  m_optionsWidget.pointsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_optionsWidget.pointsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect( m_optionsWidget.pointsTable, SIGNAL( itemSelectionChanged() ), this, SLOT( tableSelectionChanged() ) );
  connect( m_optionsWidget.pointsTable, SIGNAL( cellChanged(int,int) ), this, SLOT( tableCellChanged(int,int) ) );

  // Connect the buttons
  connect( m_optionsWidget.pushXPlus, SIGNAL(clicked()), this, SLOT(transPlusX()) );
  connect( m_optionsWidget.pushYPlus, SIGNAL(clicked()), this, SLOT(transPlusY()) );
  connect( m_optionsWidget.pushZPlus, SIGNAL(clicked()), this, SLOT(transPlusZ()) );
  connect( m_optionsWidget.pushXMinus, SIGNAL(clicked()), this, SLOT(transMinusX()) );
  connect( m_optionsWidget.pushYMinus, SIGNAL(clicked()), this, SLOT(transMinusY()) );
  connect( m_optionsWidget.pushZMinus, SIGNAL(clicked()), this, SLOT(transMinusZ()) );
  connect( m_optionsWidget.rotateXPlus, SIGNAL(clicked()), this, SLOT(rotPlusX()) );
  connect( m_optionsWidget.rotateYPlus, SIGNAL(clicked()), this, SLOT(rotPlusY()) );
  connect( m_optionsWidget.rotateZPlus, SIGNAL(clicked()), this, SLOT(rotPlusZ()) );
  connect( m_optionsWidget.rotateXMinus, SIGNAL(clicked()), this, SLOT(rotMinusX()) );
  connect( m_optionsWidget.rotateYMinus, SIGNAL(clicked()), this, SLOT(rotMinusY()) );
  connect( m_optionsWidget.rotateZMinus, SIGNAL(clicked()), this, SLOT(rotMinusZ()) );
  connect( m_optionsWidget.scaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(scaleChanged(double)) );
}

void rt3DPointBufferDataObject::cleanupGUI() {

}

int rt3DPointBufferDataObject::getNextId() {
  // Try to find the next available ID.
  for (int ix1=0; ix1<INT_MAX; ix1++) {
    if (!m_namedInfoData.contains(ix1)) {
      return ix1;
    }
  }
  return -1;
}

void rt3DPointBufferDataObject::updateColumnHeaders() {
  QHash<int, rtNamedInfoPointData>::iterator i;
  QList<QString> tagList;

  for (i = m_namedInfoData.begin(); i != m_namedInfoData.end(); ++i) {
    tagList = i.value().getTagList();
    for ( int ix1=0; ix1<tagList.size(); ix1++ ) {
      if ( !m_columnHeaderList.contains(tagList[ix1]) )
        m_columnHeaderList.append(tagList[ix1]);
    }
  }
}
