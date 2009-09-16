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
#include "SenderThread.h"
#include "SenderSimp.h"
#include <QMetaType>

SenderThread::SenderThread() {
  sender = NULL;
  calledDestructor = false;
}

SenderThread::~SenderThread() {
  calledDestructor = true;
  objLock.lock();
  if(sender) delete sender;
  this->quit();
  while(!this->wait());
  objLock.unlock();
}

//! The thread actions, which connects the signals and slots for the new thread
void SenderThread::run() {
  sender = new SenderSimp();
  connect(this, SIGNAL(connectSignal()), sender, SLOT(connectAndMessage()), Qt::QueuedConnection);
  connect(this, SIGNAL(disconnectSignal()), sender, SLOT(disconnect()), Qt::QueuedConnection);
  connect(this, SIGNAL(readAndSetDataSignal()), sender, SLOT(runReadMode()), Qt::QueuedConnection);
  objCV.wakeAll();
  exec();
}

//! Method to check if the "sender" is NULL before doing actions
void SenderThread::checkObjects() {
  if (sender == NULL) {
    objLock.lock();
    objCV.wait(&objLock);
    objLock.unlock();
  }
}

//! Read from Geometry Server and set data in Vurtigo
void SenderThread::readAndSetData() {
  checkObjects();
  emit readAndSetDataSignal();
}

//! Connect and print to console
void SenderThread::serverConnect() {
  if (!calledDestructor) {
    checkObjects();
    emit connectSignal();
  }
}


void SenderThread::serverDisconnect() {
  if (!calledDestructor) {
    checkObjects();
    emit disconnectSignal();
  }
}

//! Returns sender settings (hostname, port, etc.)
arguments * SenderThread::getArgs() {
  checkObjects();
  return sender->getArgs();
}

