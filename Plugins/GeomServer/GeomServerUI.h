#ifndef GEOM_SERVER_UI_H
#define GEOM_SERVER_UI_H

#include "ui_GeomServer.h"
#include "SenderSimp.h"

class GeomServerUI : public QWidget, private Ui::MainUI {
  Q_OBJECT

  SenderSimp & sender;

  void setupSlots();
  void setupDefaults();
  void init();

  public:
    GeomServerUI(SenderSimp & sender);
    ~GeomServerUI();

  public slots:
    void serverConnect();
    void serverDisconnect();
    void selectSendFile();
    void sendFile();
};

#endif
