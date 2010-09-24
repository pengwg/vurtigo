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
  m_pointList.append(sp);
  Modified();
}

//! Remove a point from the list
void rt3DPointBufferDataObject::removePoint(rtBasic3DPointData sp) {
  int i;

  if (m_pointList.contains(sp)) {
    i = m_pointList.indexOf(sp);
    m_pointList.removeAt(i);
    Modified();
  }
}

void rt3DPointBufferDataObject::addCartoPoint(rtCartoPointData pt) {
  m_cartoPointList.append(pt);
  Modified();
}

void rt3DPointBufferDataObject::removeCartoPoint(rtCartoPointData pt) {
  int i;

  if (m_cartoPointList.contains(pt)) {
    i = m_cartoPointList.indexOf(pt);
    m_cartoPointList.removeAt(i);
    Modified();
  }
}

//! Send the info to the GUI
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::update() {

}

//! Set the GUI widgets.
/*!
 */
void rt3DPointBufferDataObject::setupGUI() {
  m_optionsWidget.setupUi(getBaseWidget());

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

//! Clean the GUI widgets.
/*!
  @todo Write the GUI
 */
void rt3DPointBufferDataObject::cleanupGUI() {

}

void rt3DPointBufferDataObject::applyTransformToPoints(vtkTransform * t) {
  if (!t) return;

  for (int ix1=0; ix1<m_pointList.size(); ix1++) {
    m_pointList[ix1].applyTransform(t);
  }
}


void rt3DPointBufferDataObject::applyTransformToCartoPoints(vtkTransform * t) {
  if (!t) return;

  for (int ix1=0; ix1<m_cartoPointList.size(); ix1++) {
    m_cartoPointList[ix1].applyTransform(t);
  }
}


/////////////
// Public slots
/////////////


void rt3DPointBufferDataObject::transPlusX() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setX(pPoint->getX() + 10);
  Modified();
}

void rt3DPointBufferDataObject::transMinusX() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setX(pPoint->getX() - 10);
  Modified();
}

void rt3DPointBufferDataObject::transPlusY() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setY(pPoint->getY() + 10);
  Modified();
}

void rt3DPointBufferDataObject::transMinusY() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setY(pPoint->getY() - 10);
  Modified();
}

void rt3DPointBufferDataObject::transPlusZ() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setZ(pPoint->getZ() + 10);
  Modified();
}

void rt3DPointBufferDataObject::transMinusZ() {
  rtBasic3DPointData *pPoint = getPointAtIndex(0);
  pPoint->setZ(pPoint->getZ() - 10);
  Modified();
}

void rt3DPointBufferDataObject::rotPlusX() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateX(5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusX() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateX(-5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotPlusY() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateY(5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusY() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateY(-5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotPlusZ() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateZ(5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::rotMinusZ() {
  vtkTransform * temp= vtkTransform::New();
  temp->RotateZ(-5.0);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}

void rt3DPointBufferDataObject::scaleChanged(double val) {
  double scaleChange = val / m_currentScale;
  m_currentScale = val;

  vtkTransform * temp= vtkTransform::New();
  temp->Scale(scaleChange, scaleChange, scaleChange);
  applyTransformToPoints(temp);
  applyTransformToCartoPoints(temp);
  if (temp) temp->Delete();
  Modified();
}
