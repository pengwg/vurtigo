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
#ifndef DICOMREADERUI_H
#define DICOMREADERUI_H

#include <QWidget>
#include <QString>
#include <ui_DICOMReader.h>

#include <vtkSmartPointer.h>

#include "DICOMFileReader.h"

class DICOMReaderUI : public QWidget, public Ui::DICOMReader
{
  Q_OBJECT

public:
  DICOMReaderUI();
  ~DICOMReaderUI();

public slots:
  void newDirectory();
  void dirChooser();
  void saveAsVolume();
  void spacingChanged() {m_customReader.setSpacing(spacingX->value(),spacingY->value(),spacingZ->value());}

protected:
  void connectSignals();

  QString m_lastDir;

  DICOMFileReader m_customReader;

  //! The ID of the newest volume
  int m_vol;

  //! The volume is a DICOM
  bool isDICOM;
};

#endif // DICOMREADERUI_H
