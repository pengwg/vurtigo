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
#include "JpgWriter.h"
#include "rtBaseHandle.h"
#include "ScreenCaptureThread.h"
#include "vtkImageData.h"
#include "vtkAlgorithmOutput.h"

#include <iostream>


JpgWriter::JpgWriter(ScreenCaptureThread* thread)
{
  m_currImg = 0;
  m_snapTimer = NULL;
  m_capThread = thread;
  m_jpegQuality = 80;
  m_updateInterval = 50;
  m_timerValid = false;

  m_jpegWriter = vtkJPEGWriter::New();
}

JpgWriter::~JpgWriter() {
  m_jpegWriter->Delete();
}

//! Set a new file save name.
/*!
  This function also resets the current image to zero.
  */
void JpgWriter::setFileName(QString fName) {
  m_fileName = fName;
  m_currImg = 0;
  emit newSeriesImage(m_currImg);
}

//! Take a quick screenshot.
void JpgWriter::screenshot() {
  if (m_fileName == "") return;

  vtkImageData* temp = vtkImageData::New();
  QString localFileName = m_fileName + ".jpg";

  emit requestNewScreen();
  m_capThread->getScreenCopy(temp);

  m_jpegWriter->SetInputConnection(0, temp->GetProducerPort());
  m_jpegWriter->SetFileName( localFileName.toStdString().c_str() );
  m_jpegWriter->SetQuality(m_jpegQuality);
  m_jpegWriter->Write();

  temp->Delete();
}

void JpgWriter::seriesImage() {
  if (m_fileName == "") return;

  vtkImageData* temp = vtkImageData::New();
  QString numStr;

  numStr.sprintf("%05d", m_currImg);

  QString localFileName = m_fileName + numStr + ".jpg";

  emit requestNewScreen();
  m_capThread->getScreenCopy(temp);

  m_jpegWriter->SetInputConnection(0, temp->GetProducerPort());
  m_jpegWriter->SetFileName( localFileName.toStdString().c_str() );
  m_jpegWriter->SetQuality(m_jpegQuality);
  m_jpegWriter->Write();

  temp->Delete();

  m_currImg++;
  emit newSeriesImage(m_currImg);
}

void JpgWriter::startSeries() {
  m_snapTimer = new QTimer();
  connect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  m_snapTimer->setInterval(m_updateInterval);
  m_snapTimer->start();
  m_timerValid = true;
}

void JpgWriter::pauseSeries(bool state) {
  if (!m_timerValid || !m_snapTimer) return;
  if (state) {
    m_snapTimer->stop();
  } else {
    m_snapTimer->start();
  }
}

void JpgWriter::endSeries() {
  m_timerValid = false;
  if (m_snapTimer->isActive()) {
    m_snapTimer->stop();
  }
  disconnect(m_snapTimer, SIGNAL(timeout()), this, SLOT(seriesImage()));
  delete m_snapTimer;
  m_snapTimer = NULL;
}
