#ifndef SCREEN_CAPTURE_UI_H
#define SCREEN_CAPTURE_UI_H

#include <QWidget>
#include <QString>
#include <QtGlobal>
#include <QThread>

#include <ui_ScreenCapture.h>
#include "ScreenCaptureThread.h"


class ScreenCaptureUI : public QWidget, public Ui::ScreenCapture
{
  Q_OBJECT

public:
  ScreenCaptureUI();
  ~ScreenCaptureUI();

  //! Get the root name for the file to be saved.
  QString getFileRoot() { return m_lastDir; }

public slots:
  void newFile();
  void fileChooser();
  void jpegQualityChanged(int);
  void jpegIntervalChanged(int);
  void jpegStart();
  void jpegPause(bool);
  void jpegStop();
  void moreImagesTaken(int);

protected:
  void connectSignals();

  QString m_lastDir;
  ScreenCaptureThread m_captureThread;
};

#endif
