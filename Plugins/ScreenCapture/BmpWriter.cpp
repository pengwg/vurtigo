#include "BmpWriter.h"
#include "rtBaseHandle.h"
#include "ScreenCaptureThread.h"
#include "vtkImageData.h"
#include "vtkAlgorithmOutput.h"

#include <iostream>


BmpWriter::BmpWriter(ScreenCaptureThread* thread)
{
  m_currImg = 0;
  m_snapTimer = NULL;
  m_capThread = thread;
  m_updateInterval = 50;
  m_timerValid = false;

  m_bmpWriter = vtkBMPWriter::New();
}

BmpWriter::~BmpWriter() {
  m_bmpWriter->Delete();
}

//! Set a new file save name.
/*!
  This function also resets the current image to zero.
  */
void BmpWriter::setFileName(QString fName) {
  m_fileName = fName;
  m_currImg = 0;
  emit newSeriesImage(m_currImg);
}

//! Take a quick screenshot.
void BmpWriter::screenshot() {
  if (m_fileName == "") return;

  vtkImageData* temp = vtkImageData::New();
  QString localFileName = m_fileName + ".bmp";

  emit requestNewScreen();
  m_capThread->getScreenCopy(temp);

  m_bmpWriter->SetInputConnection(0, temp->GetProducerPort());
  m_bmpWriter->SetFileName( localFileName.toStdString().c_str() );
  m_bmpWriter->Write();

  temp->Delete();
}

void BmpWriter::seriesImage() {
  if (m_fileName == "") return;

  vtkImageData* temp = vtkImageData::New();
  QString numStr;

  numStr.sprintf("%05d", m_currImg);

  QString localFileName = m_fileName + numStr + ".bmp";

  emit requestNewScreen();
  m_capThread->getScreenCopy(temp);

  m_bmpWriter->SetInputConnection(0, temp->GetProducerPort());
  m_bmpWriter->SetFileName( localFileName.toStdString().c_str() );
  m_bmpWriter->Write();

  temp->Delete();

  m_currImg++;
  emit newSeriesImage(m_currImg);
}

void BmpWriter::startSeries() {
  m_snapTimer = new QTimer();
  connect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  m_snapTimer->setInterval(m_updateInterval);
  m_snapTimer->start();
  m_timerValid = true;
}

void BmpWriter::pauseSeries(bool state) {
  if (!m_timerValid || !m_snapTimer) return;
  if (state) {
    m_snapTimer->stop();
  } else {
    m_snapTimer->start();
  }
}

void BmpWriter::endSeries() {
  m_timerValid = false;
  if (m_snapTimer->isActive()) {
    m_snapTimer->stop();
  }
  disconnect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  delete m_snapTimer;
  m_snapTimer = NULL;
}
