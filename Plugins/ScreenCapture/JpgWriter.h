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
#ifndef JPGWRITER_H
#define JPGWRITER_H

#include <QThread>
#include <QtGlobal>
#include <QTimer>

#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>

class ScreenCaptureThread;

class JpgWriter : public QObject
{
  Q_OBJECT

public:
  JpgWriter(ScreenCaptureThread* thread);
  ~JpgWriter();

  void setFileName(QString fName);

  int getNumImages() { return m_currImg; }

public slots:
  void screenshot();
  void setQuality(int quality) { m_jpegQuality=quality; }
  void setInterval(int interval) { m_updateInterval=interval; }
  void startSeries();
  void pauseSeries(bool);
  void endSeries();

  void seriesImage();

signals:
  //! Signal sent when requesting a new image.
  void requestNewScreen();
  //! Signal sent when a new image has been added to the series.
  void newSeriesImage(int);

protected:
  quint16 m_currImg;
  vtkSmartPointer<vtkJPEGWriter> m_jpegWriter;
  QString m_fileName;
  ScreenCaptureThread* m_capThread;
  int m_jpegQuality;

  // For the image series.
  QTimer* m_snapTimer;
  int m_updateInterval;
  bool m_timerValid;
};

#endif // JPGWRITER_H
