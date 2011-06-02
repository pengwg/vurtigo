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
