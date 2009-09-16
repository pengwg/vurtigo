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
#ifndef SCREEN_CAPTURE_UI_H
#define SCREEN_CAPTURE_UI_H

#include <QWidget>
#include <QString>
#include <QtGlobal>
#include <QThread>

#include <ui_ScreenCapture.h>
#include "ScreenCaptureThread.h"


class ScreenCaptureUI : public QWidget, public Ui::ScreenCapture
{
  Q_OBJECT

public:
  ScreenCaptureUI();
  ~ScreenCaptureUI();

  //! Get the root name for the file to be saved.
  QString getFileRoot() { return m_lastDir; }

public slots:
  void newFile();
  void fileChooser();

  // JPEG slots
  void jpegQualityChanged(int);
  void jpegIntervalChanged(int);
  void jpegStart();
  void jpegPause(bool);
  void jpegStop();
  void jpegMoreImagesTaken(int);

  // BMP slots
  void bmpIntervalChanged(int);
  void bmpStart();
  void bmpPause(bool);
  void bmpStop();
  void bmpMoreImagesTaken(int);

  // AVI slots
  void aviIntervalChanged(int);
  void aviStart();
  void aviPause(bool);
  void aviStop();
  void aviMoreImagesTaken(int);

protected:
  void connectSignals();

  QString m_lastDir;
  ScreenCaptureThread m_captureThread;
};

#endif
