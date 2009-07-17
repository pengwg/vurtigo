#ifndef AVIWRITER_H
#define AVIWRITER_H

#include <QThread>
#include <QtGlobal>
#include <QTimer>

#include <vtkSmartPointer.h>
#include <vtkFFMPEGWriter.h>

class ScreenCaptureThread;

class AviWriter : public QObject
{
  Q_OBJECT

public:
  AviWriter(ScreenCaptureThread* thread);
  ~AviWriter();

  void setFileName(QString fName);

  int getNumImages() { return m_currImg; }

public slots:
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
  vtkSmartPointer<vtkFFMPEGWriter> m_aviWriter;
  QString m_fileName;
  ScreenCaptureThread* m_capThread;

  // For the image series.
  QTimer* m_snapTimer;
  int m_updateInterval;
  bool m_timerValid;
  vtkSmartPointer<vtkImageData> m_localImg;
};

#endif // AVIWRITER_H
