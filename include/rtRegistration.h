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
#ifndef RTREGISTRATION_H
#define RTREGISTRATION_H

#include "ui_landmarkRegistration.h"


#include <QMouseEvent>
#include <QColor>

class rt3DPointBufferDataObject;

class rtRegistration : public QDialog, private Ui::landmarkRegistration {
Q_OBJECT

 public:


  rtRegistration(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtRegistration();


 public slots:
  void registerButtonPressed();
  // set the names of the 3D point objects in the combo boxes
  void setupPointCombos();
  void setupVolumeCombos();
  void setupAllCombos();
  // setup the table values with points
  void setupSourceTable();
  void setupTargetTable();


  void sourceRadioChosen() {m_activeSet = 0;}
  void targetRadioChosen() {m_activeSet = 1;}

  void addActivePoint(QMouseEvent*);
  void addNewPoints();

  void placementOff();
  void placementOn();

protected:

 //! The list of all 3D point objects
 QList<int> m_points;

 //! The list of all 3D volume objects
 QList<int> m_volumes;

 //! the active point set
 int m_activeSet;

 //! The current color
 QColor m_color;
 QStringList m_colorList;


};


#endif // RTREGISTRATION_H
