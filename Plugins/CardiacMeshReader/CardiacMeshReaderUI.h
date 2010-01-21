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
#ifndef CARDIAC_MESH_READERUI_H
#define CARDIAC_MESH_READERUI_H

#include <QWidget>
#include <QString>
#include <ui_CardiacMeshReader.h>

#include <vtkSmartPointer.h>

#include "DICOMFileReader.h"

class CardiacMeshReaderUI : public QWidget, public Ui::CardiacMeshReader
{
  Q_OBJECT

public:
  CardiacMeshReaderUI();
  ~CardiacMeshReaderUI();

public slots:
  void newDirectory();
  void dirChooser();
  void saveAsVolume();

protected:
  void connectSignals();

  QString m_lastDir;

  DICOMFileReader m_customReader;

  //! The ID of the newest volume
  int m_vol;
};

#endif
