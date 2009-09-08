#ifndef GEOM_SERVER_UI_H
#define GEOM_SERVER_UI_H

#include "ui_GeomServer.h"
#include "SenderThread.h"
#include <QList>

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

  protected:
    QList<SenderThread::ListObject> m_planeList;
    QList<SenderThread::ListObject> m_cathList;
    int m_numPlanes, m_numCath;

    void updateObjectList();
    void updateListGUI();
    void updateReaderObject();

  public:
    GeomServerUI(SenderThread & senderThread);
    ~GeomServerUI();

  public slots:
    void serverConnect();
    void serverDisconnect();
    void numPlanesChanged(int i);
    void numCathChanged(int i);
    void itemStateChanged(int);
};

#endif
