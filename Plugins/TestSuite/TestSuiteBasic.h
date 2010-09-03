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
#ifndef TESTSUITEBASIC_H
#define TESTSUITEBASIC_H

#include <QThread>
#include <QTimer>

#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>

class TestSuiteBasic : public QThread
{
  Q_OBJECT

public:
  TestSuiteBasic();
  ~TestSuiteBasic();

  void run();

  inline void setPngFileName(QString name) { m_pngFileName=name; }
  inline void setDicomFileName(QString name) { m_dicomFileName=name; }

public slots:
  void changeImage();
signals:
  void sendOutput( QString );

protected:
  void basicTestCreateObjects();
  void testObject(int, QString);

  QString m_pngFileName;
  QString m_dicomFileName;

  QTimer m_imgChange;
  int m_imgPeriod;

  // Object IDs
  int m_label;
  int m_3DPoints;
  int m_cath[3];
  int m_smallVol;
  int m_hugeVol;
  int m_2DPlane;
  int m_2DPlaneColor;
  int m_2DPlane16;
  int m_matrix;
  int m_polyObj;
  int m_ctf, m_ctfGreen;
  int m_piece;
  
  vtkImageMapToColors       *m_imgMapToColors;
  vtkWindowLevelLookupTable *m_lookupTable;

};

#endif // TESTSUITEBASIC_H
