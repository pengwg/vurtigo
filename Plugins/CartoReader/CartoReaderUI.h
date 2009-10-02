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
#ifndef CARTO_READER_UI_H
#define CARTO_READER_UI_H

#include <QWidget>
#include <QString>
#include <ui_CartoReader.h>


#include "CartoFileReader.h"

class CartoReaderUI : public QWidget, public Ui::CartoReader
{
  Q_OBJECT

public:
  CartoReaderUI();
  ~CartoReaderUI();

public slots:
  void loadFile();
  void saveAsPoints();
  void saveAsSurface();

protected:
  void connectSignals();

  //! Set the info in the table based on the reader.
  void resetTableInfo();

  //! Name of the loaded file.
  QString m_fName;
  CartoFileReader m_customReader;

  //! The ID of the points set object.
  int m_pts;
};

#endif
