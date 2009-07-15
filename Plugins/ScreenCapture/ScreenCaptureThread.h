#ifndef SCREENCAPTURETHREAD_H
#define SCREENCAPTURETHREAD_H

#include <QThread>
#include <QSemaphore>

#include "JpgWriter.h"
#include "vtkImageData.h"

class ScreenCaptureThread : public QThread
{
  Q_OBJECT

  public:
    ScreenCaptureThread();
    ~ScreenCaptureThread();

    void run();

    //! Get the pointer to the jpg writer object.
    JpgWriter* getJpgWriter() { return m_jpgWriter; }

    void setFileName(QString fName);
    void waitForObjectInit();
    void getScreenCopy(vtkImageData* copy);

  public slots:
    void updateScreenCapture();

  protected:
    JpgWriter* m_jpgWriter;
    QSemaphore m_objectsCreated;
    QSemaphore m_screenLock;
    vtkImageData* m_screen;
};

#endif // SCREENCAPTURETHREAD_H
