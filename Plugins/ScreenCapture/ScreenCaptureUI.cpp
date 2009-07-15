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

  moreImagesTaken(m_captureThread.getJpgWriter()->getNumImages());
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

  connect(m_captureThread.getJpgWriter(), SIGNAL(newSeriesImage(int)), this, SLOT(moreImagesTaken(int)));

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

  dir = QFileDialog::getSaveFileName(this, "Save File Prefix");
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

void ScreenCaptureUI::moreImagesTaken(int num) {
  jpgImgsTaken->setText(QString::number(num));
}
