#include "AviWriter.h"
#include "rtBaseHandle.h"
#include "ScreenCaptureThread.h"
#include "vtkImageData.h"
#include "vtkAlgorithmOutput.h"

#include <iostream>


AviWriter::AviWriter(ScreenCaptureThread* thread)
{
  m_currImg = 0;
  m_snapTimer = NULL;
  m_capThread = thread;
  m_updateInterval = 50;
  m_timerValid = false;

  m_aviWriter = vtkFFMPEGWriter::New();
  m_localImg = vtkImageData::New();
}

AviWriter::~AviWriter() {
  m_aviWriter->Delete();
  m_localImg->Delete();
}

//! Set a new file save name.
/*!
  This function also resets the current frame to zero.
  */
void AviWriter::setFileName(QString fName) {
  m_fileName = fName;
  m_currImg = 0;
  emit newSeriesImage(m_currImg);
}

void AviWriter::seriesImage() {
  emit requestNewScreen();
  m_capThread->getScreenCopy(m_localImg);

  m_aviWriter->SetInputConnection(0, m_localImg->GetProducerPort());
  m_aviWriter->Write();

  m_currImg++;
  emit newSeriesImage(m_currImg);
}

void AviWriter::startSeries() {
  if (m_fileName == "") return;

  QString localFileName = m_fileName + ".avi";
  m_aviWriter->SetFileName( localFileName.toStdString().c_str() );
  m_aviWriter->SetQuality(2); // Best quality.
  m_aviWriter->SetRate( (500.0f/(double)m_updateInterval)+1 ); // Record at a slightly slower frame rate

  // Set the first frame
  emit requestNewScreen();
  m_capThread->getScreenCopy(m_localImg);

  m_aviWriter->SetInputConnection(0, m_localImg->GetProducerPort());

  // Start the recording.
  m_aviWriter->Start();

  m_snapTimer = new QTimer();
  connect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  m_snapTimer->setInterval(m_updateInterval);
  m_snapTimer->start();
  m_timerValid = true;
}

void AviWriter::pauseSeries(bool state) {
  if (!m_timerValid || !m_snapTimer) return;
  if (state) {
    m_snapTimer->stop();
  } else {
    m_snapTimer->start();
  }
}

void AviWriter::endSeries() {
  if (!m_snapTimer || !m_aviWriter) return;

  m_timerValid = false;
  if (m_snapTimer->isActive()) {
    m_snapTimer->stop();
  }
  disconnect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  delete m_snapTimer;
  m_snapTimer = NULL;

  m_aviWriter->End();
}
