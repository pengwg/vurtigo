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
#ifndef SCREENCAPTURETHREAD_H
#define SCREENCAPTURETHREAD_H

#include <QThread>
#include <QSemaphore>

#include "JpgWriter.h"
#include "BmpWriter.h"
#include "AviWriter.h"
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
    BmpWriter* getBmpWriter() { return m_bmpWriter; }
    AviWriter* getAviWriter() { return m_aviWriter; }

    void setFileName(QString fName);
    void waitForObjectInit();
    void getScreenCopy(vtkImageData* copy);

  public slots:
    void updateScreenCapture();

  protected:
    JpgWriter* m_jpgWriter;
    BmpWriter* m_bmpWriter;
    AviWriter* m_aviWriter;

    QSemaphore m_objectsCreated;
    QSemaphore m_screenLock;
    vtkImageData* m_screen;
};

#endif // SCREENCAPTURETHREAD_H
