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

#include "DICOMFileReader.h"
#include "MeshFileReader.h"
#include "MeshPointSet.h"

class rtPolyDataObject;

class CardiacMeshReaderUI : public QWidget, public Ui::CardiacMeshReader
{
  Q_OBJECT

public:
  CardiacMeshReaderUI();
  ~CardiacMeshReaderUI();

public slots:
  void newDirectory();
  void dirChooser();
  void page1Next();
  void page1Finish();

  void newDirectoryMesh();
  void dirChooserMesh();
  void minSliderChanged(int);
  void maxSliderChanged(int);
  void page2Next();
  void page2Back();
  void page2Finish();

  void namesChanged();
  void page3Back();
  void page3Finish();

protected:
  void connectSignals();

  bool loadPolyDataFromPoints(rtPolyDataObject* data, MeshPointSet::PointType type, double smoothStep=1.0);

  QString m_lastDir;
  QString m_lastMeshDir;

  DICOMFileReader m_customReader;
  MeshFileReader m_meshReader;

  //! The ID of the newest volume
  int m_vol;

  //! The ID of the poly data icontour
  int m_icontour;

  //! The ID of the non smooth icontour
  int m_icontourNoSmooth;

  //! The ID of the poly data ocontour
  int m_ocontour;

  //! The ID of the non smooth ocontour
  int m_ocontourNoSmooth;
};

#endif
