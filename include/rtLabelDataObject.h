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
#ifndef RT_LABEL_DATA_OBJECT_H
#define RT_LABEL_DATA_OBJECT_H

// Local Includes
#include "rtDataObject.h"

// VTK includes
#include "vtkTextProperty.h"

// Qt Includes
#include <QString>
#include <QLabel>
#include <QPushButton>

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;

class rtLabelDataObject : public rtDataObject {

 Q_OBJECT

 public:
  rtLabelDataObject();
  ~rtLabelDataObject();

  void setText(QString text);
  QString getText();

  void setColor(double, double, double);
  void setOpacity(double);

  vtkTextProperty* getTextProperty();

  void update();

 public slots:
  void propertyDialog();
  void textChanged(QString);

 protected:
  //Properties
  QString m_labelText;
  vtkTextProperty* m_textProp;

  //GUI elements

  //! The master grid layout
  QGridLayout* m_masterLayout;
  //! Just a title label.
  QLabel* m_nameLabel;

  ////
  // Text Choice 
  ////
  //! Label for text choice
  QLabel m_chooseTextLabel;
  //! Edit box for text choice
  QLineEdit* m_chooseTextEdit;

  ////
  // Colour Choice
  ////
  QLabel m_propertyLabel;
  QPushButton m_propertyButton;

  // Functions
  void setupGUI();
  void cleanupGUI();

 private:
};

#endif
