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
#ifndef SENDER_THREAD_H
#define SENDER_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "arguments.h"

class SenderSimp;

//! Thread class to work with a server on a different thread
class SenderThread : public QThread {
  Q_OBJECT

  public:
  //! Different types of objects.
  enum ObjectTypes {
    OBJ_TYPE_PLANE,
    OBJ_TYPE_CATH
  };

  //! The different options for each object.
  enum ObjectActions {
    OBJ_IGNORE,
    OBJ_READ,
    OBJ_WRITE,
    OBJ_ERROR
  };

  struct ListObject {
    int id;
    ObjectTypes type;
    ObjectActions act;
  };

  SenderThread();
  ~SenderThread();

  //! Function to start the thread.
  void run();

  //! Read from Geometry Server and set data in Vurtigo
  void readAndSetData();

  //! Connect the the geom server
  void serverConnect();

  //! Disconnect from the server
  void serverDisconnect();

  //! Returns sender settings (hostname, port, etc.)
  arguments * getArgs();

  //! Get the handle to the sender object.
  SenderSimp* getSender() { return sender; }

protected:
  //! To communicate geometry server
  SenderSimp *sender;

  //! Locks until the SenderSimp object is no longer NULL
  QMutex objLock;
  //! Stops waiting when the SenderSimp object is no longer NULL
  QWaitCondition objCV;

  //! Method to check if the "sender" is NULL before doing actions
  void checkObjects();

  //! true when destructor is called and flase otherwise, helps ensure that the thread is not running when being destroyed
  bool m_calledDestructor;

signals:
  // Sender signals for multithreading
  void readAndSetDataSignal();
  void connectSignal();
  void disconnectSignal();
};

#endif
