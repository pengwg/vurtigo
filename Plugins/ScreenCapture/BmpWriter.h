#ifndef BMPWRITER_H
#define BMPWRITER_H

#include <QThread>
#include <QtGlobal>
#include <QTimer>

#include <vtkSmartPointer.h>
#include <vtkBMPWriter.h>

class ScreenCaptureThread;

class BmpWriter : public QObject
{
  Q_OBJECT

public:
  BmpWriter(ScreenCaptureThread* thread);
  ~BmpWriter();

  void setFileName(QString fName);

  int getNumImages() { return m_currImg; }

public slots:
  void screenshot();
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
  vtkSmartPointer<vtkBMPWriter> m_bmpWriter;
  QString m_fileName;
  ScreenCaptureThread* m_capThread;

  // For the image series.
  QTimer* m_snapTimer;
  int m_updateInterval;
  bool m_timerValid;
};

#endif // BMPWRITER_H
