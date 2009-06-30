#ifndef GEOM_SERVER_UI_H
#define GEOM_SERVER_UI_H

#include "ui_GeomServer.h"
#include "SenderThread.h"

//! User interface for the GeomServer plugin
/*!
  Handles the logic  and workflow relating to the user interface
  */
class GeomServerUI : public QWidget, private Ui::MainUI {
  Q_OBJECT

  //! Wrapper for sending information
  SenderThread & senderThread;

  void setupSlots();
  void setupDefaults();
  void init();
  SenderSimp::TxtFileType checkboxTxtFileType();

  public:
    GeomServerUI(SenderThread & senderThread);
    ~GeomServerUI();

  public slots:
    void serverConnect();
    void serverDisconnect();
    void selectSendFile();
    void sendFile();
};

#endif
