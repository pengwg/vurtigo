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
#include "ScreenCaptureUI.h"

#include <QFileDialog>

#include <iostream>

#include "rtBaseHandle.h"

ScreenCaptureUI::ScreenCaptureUI() {
  setupUi(this);

  m_lastDir = "";

  // Start the thread and wait for the thread to finish creating objects.
  m_captureThread.start();
  m_captureThread.waitForObjectInit();

  connectSignals();

  // JPEG Init
  // Quality at 80%.
  jpgQualitySlider->setValue(80);
  jpegQualityChanged(80);
  m_captureThread.getJpgWriter()->setQuality(80);

  // Set Interval to 50 ms.
  jpgIntervalSlider->setValue(50);
  jpegIntervalChanged(50);
  m_captureThread.getJpgWriter()->setInterval(50);

  // Only the start button shoud be usable at the time of init.
  jpgStartBtn->setEnabled(true);
  jpgPauseBtn->setEnabled(false);
  jpgStopBtn->setEnabled(false);

  jpegMoreImagesTaken(m_captureThread.getJpgWriter()->getNumImages());

  // BMP Init
  // Set Interval to 50 ms.
  bmpIntervalSlider->setValue(50);
  bmpIntervalChanged(50);
  m_captureThread.getBmpWriter()->setInterval(50);

  // Only the start button shoud be usable at the time of init.
  bmpStartBtn->setEnabled(true);
  bmpPauseBtn->setEnabled(false);
  bmpStopBtn->setEnabled(false);

  bmpMoreImagesTaken(m_captureThread.getBmpWriter()->getNumImages());

  // AVI Init
  // Set Interval to 50 ms.
  aviIntervalSlider->setValue(50);
  aviIntervalChanged(50);
  m_captureThread.getAviWriter()->setInterval(50);

  // Only the start button shoud be usable at the time of init.
  aviStartBtn->setEnabled(true);
  aviPauseBtn->setEnabled(false);
  aviStopBtn->setEnabled(false);

  aviMoreImagesTaken(m_captureThread.getAviWriter()->getNumImages());
}

ScreenCaptureUI::~ScreenCaptureUI() {
  m_captureThread.exit();
  m_captureThread.wait();
}

void ScreenCaptureUI::connectSignals() {
  // Choose a new save file.
  connect(fileNameBtn, SIGNAL(clicked()), this, SLOT(fileChooser()));

  // The edit box was changed directly.
  connect(fileNameEdit, SIGNAL(returnPressed()), this, SLOT(newFile()));

  // Connect the signals to the jpg writer.
  connect(jpgScreenCapBtn, SIGNAL(clicked()), m_captureThread.getJpgWriter(), SLOT(screenshot()), Qt::QueuedConnection);
  connect(jpgQualitySlider, SIGNAL(valueChanged(int)), m_captureThread.getJpgWriter(), SLOT(setQuality(int)));
  connect(jpgQualitySlider, SIGNAL(valueChanged(int)), this, SLOT(jpegQualityChanged(int)));
  connect(jpgIntervalSlider, SIGNAL(valueChanged(int)), m_captureThread.getJpgWriter(), SLOT(setInterval(int)));
  connect(jpgIntervalSlider, SIGNAL(valueChanged(int)), this, SLOT(jpegIntervalChanged(int)));

  connect(jpgStartBtn, SIGNAL(clicked()), m_captureThread.getJpgWriter(), SLOT(startSeries()));
  connect(jpgStartBtn, SIGNAL(clicked()), this, SLOT(jpegStart()));
  connect(jpgPauseBtn, SIGNAL(toggled(bool)), m_captureThread.getJpgWriter(), SLOT(pauseSeries(bool)));
  connect(jpgPauseBtn, SIGNAL(toggled(bool)), this, SLOT(jpegPause(bool)));
  connect(jpgStopBtn, SIGNAL(clicked()), m_captureThread.getJpgWriter(), SLOT(endSeries()));
  connect(jpgStopBtn, SIGNAL(clicked()), this, SLOT(jpegStop()));

  connect(m_captureThread.getJpgWriter(), SIGNAL(newSeriesImage(int)), this, SLOT(jpegMoreImagesTaken(int)));

  // Connect signals and slots for the bmp writer.
  connect(bmpScreenCapBtn, SIGNAL(clicked()), m_captureThread.getBmpWriter(), SLOT(screenshot()), Qt::QueuedConnection);
  connect(bmpIntervalSlider, SIGNAL(valueChanged(int)), m_captureThread.getBmpWriter(), SLOT(setInterval(int)));
  connect(bmpIntervalSlider, SIGNAL(valueChanged(int)), this, SLOT(bmpIntervalChanged(int)));

  connect(bmpStartBtn, SIGNAL(clicked()), m_captureThread.getBmpWriter(), SLOT(startSeries()));
  connect(bmpStartBtn, SIGNAL(clicked()), this, SLOT(bmpStart()));
  connect(bmpPauseBtn, SIGNAL(toggled(bool)), m_captureThread.getBmpWriter(), SLOT(pauseSeries(bool)));
  connect(bmpPauseBtn, SIGNAL(toggled(bool)), this, SLOT(bmpPause(bool)));
  connect(bmpStopBtn, SIGNAL(clicked()), m_captureThread.getBmpWriter(), SLOT(endSeries()));
  connect(bmpStopBtn, SIGNAL(clicked()), this, SLOT(bmpStop()));

  connect(m_captureThread.getBmpWriter(), SIGNAL(newSeriesImage(int)), this, SLOT(bmpMoreImagesTaken(int)));

  // Connect signals and slots for the avi writer.
  connect(aviIntervalSlider, SIGNAL(valueChanged(int)), m_captureThread.getAviWriter(), SLOT(setInterval(int)));
  connect(aviIntervalSlider, SIGNAL(valueChanged(int)), this, SLOT(aviIntervalChanged(int)));

  connect(aviStartBtn, SIGNAL(clicked()), m_captureThread.getAviWriter(), SLOT(startSeries()));
  connect(aviStartBtn, SIGNAL(clicked()), this, SLOT(aviStart()));
  connect(aviPauseBtn, SIGNAL(toggled(bool)), m_captureThread.getAviWriter(), SLOT(pauseSeries(bool)));
  connect(aviPauseBtn, SIGNAL(toggled(bool)), this, SLOT(aviPause(bool)));
  connect(aviStopBtn, SIGNAL(clicked()), m_captureThread.getAviWriter(), SLOT(endSeries()));
  connect(aviStopBtn, SIGNAL(clicked()), this, SLOT(aviStop()));

  connect(m_captureThread.getAviWriter(), SIGNAL(newSeriesImage(int)), this, SLOT(aviMoreImagesTaken(int)));

}

void ScreenCaptureUI::jpegQualityChanged(int val) {
  jpgQualityLabel->setText(QString::number(val)+" %");
}

void ScreenCaptureUI::jpegIntervalChanged(int val) {
  jpgIntervalLabel->setText(QString::number(val)+"  msec.");
}

//! Slot called when the user changes the directory.
void ScreenCaptureUI::newFile() {
  if(m_lastDir != fileNameEdit->text()) {
    m_lastDir = fileNameEdit->text();
    m_captureThread.setFileName(m_lastDir);
  }
}

//! Popup a dialog box to choose a directory
void ScreenCaptureUI::fileChooser() {
  QString dir;

  dir = QFileDialog::getSaveFileName(this, "Save File Prefix",m_lastDir);
  if (dir != "") {
    // The user did select a directory.
    fileNameEdit->setText(dir);
    newFile();
  }
}

//! Setup the GUI for the start.
void ScreenCaptureUI::jpegStart() {
  jpgStartBtn->setEnabled(false);
  jpgQualitySlider->setEnabled(false);
  jpgIntervalSlider->setEnabled(false);
  jpgScreenCapBtn->setEnabled(false);
  fileNameEdit->setEnabled(false);
  fileNameBtn->setEnabled(false);

  jpgPauseBtn->setEnabled(true);
  jpgPauseBtn->setChecked(false);
  jpgStopBtn->setEnabled(true);
}

void ScreenCaptureUI::jpegPause(bool state) {
}

void ScreenCaptureUI::jpegStop() {
  jpgStartBtn->setEnabled(true);
  jpgQualitySlider->setEnabled(true);
  jpgIntervalSlider->setEnabled(true);
  jpgScreenCapBtn->setEnabled(true);
  fileNameEdit->setEnabled(true);
  fileNameBtn->setEnabled(true);

  jpgPauseBtn->setEnabled(false);
  jpgStopBtn->setEnabled(false);
  jpgPauseBtn->setChecked(false);
  jpgStartBtn->setChecked(false);
}

void ScreenCaptureUI::jpegMoreImagesTaken(int num) {
  jpgImgsTaken->setText(QString::number(num));
}


//////
// BMP
//////

//! Setup the GUI for the start for BMP.
void ScreenCaptureUI::bmpStart() {
  bmpStartBtn->setEnabled(false);
  bmpIntervalSlider->setEnabled(false);
  bmpScreenCapBtn->setEnabled(false);
  fileNameEdit->setEnabled(false);
  fileNameBtn->setEnabled(false);

  bmpPauseBtn->setEnabled(true);
  bmpPauseBtn->setChecked(false);
  bmpStopBtn->setEnabled(true);
}

void ScreenCaptureUI::bmpPause(bool state) {
}

void ScreenCaptureUI::bmpStop() {
  bmpStartBtn->setEnabled(true);
  bmpIntervalSlider->setEnabled(true);
  bmpScreenCapBtn->setEnabled(true);
  fileNameEdit->setEnabled(true);
  fileNameBtn->setEnabled(true);

  bmpPauseBtn->setEnabled(false);
  bmpStopBtn->setEnabled(false);
  bmpPauseBtn->setChecked(false);
  bmpStartBtn->setChecked(false);
}
void ScreenCaptureUI::bmpIntervalChanged(int val) {
  bmpIntervalLabel->setText(QString::number(val)+"  msec.");
}

void ScreenCaptureUI::bmpMoreImagesTaken(int num) {
  bmpImgsTaken->setText(QString::number(num));
}

//////
// AVI
//////

//! Setup the GUI for the start for AVI.
void ScreenCaptureUI::aviStart() {
  aviStartBtn->setEnabled(false);
  aviIntervalSlider->setEnabled(false);
  fileNameEdit->setEnabled(false);
  fileNameBtn->setEnabled(false);

  aviPauseBtn->setEnabled(true);
  aviPauseBtn->setChecked(false);
  aviStopBtn->setEnabled(true);
}

void ScreenCaptureUI::aviPause(bool state) {
}

void ScreenCaptureUI::aviStop() {
  aviStartBtn->setEnabled(true);
  aviIntervalSlider->setEnabled(true);
  fileNameEdit->setEnabled(true);
  fileNameBtn->setEnabled(true);

  aviPauseBtn->setEnabled(false);
  aviStopBtn->setEnabled(false);
  aviPauseBtn->setChecked(false);
  aviStartBtn->setChecked(false);
}

void ScreenCaptureUI::aviIntervalChanged(int val) {
  aviIntervalLabel->setText(QString::number(val)+"  msec.");
}

void ScreenCaptureUI::aviMoreImagesTaken(int num) {
  aviImgsTaken->setText(QString::number(num));
}
