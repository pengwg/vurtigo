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
    void cathZeroOnlyChanged(bool);
};

#endif
