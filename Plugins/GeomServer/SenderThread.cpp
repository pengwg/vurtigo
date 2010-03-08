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
//#include <QMetaType>
//#include <iostream>

SenderThread::SenderThread() : sender(NULL), m_calledDestructor(false) {
}

SenderThread::~SenderThread() {
  m_calledDestructor = true;
  objLock.lock();
  if(sender) delete sender;
  this->quit();
  this->wait();
  objLock.unlock();
}

void SenderThread::run() {
  sender = new SenderSimp();
  connect(this, SIGNAL(connectSignal()), sender, SLOT(connectAndMessage()), Qt::QueuedConnection);
  connect(this, SIGNAL(disconnectSignal()), sender, SLOT(disconnect()), Qt::QueuedConnection);
  connect(this, SIGNAL(readAndSetDataSignal()), sender, SLOT(runReadMode()), Qt::QueuedConnection);
  objCV.wakeAll();
  exec();
}

void SenderThread::checkObjects() {
  if (sender == NULL) {
    objLock.lock();
    objCV.wait(&objLock);
    objLock.unlock();
  }
}

void SenderThread::readAndSetData() {
  checkObjects();
  // Must allocate a signal before sending it.
  // Do not want to overwhelm the other thread.
  if (sender->allocateNewSignal()) {
    emit readAndSetDataSignal();
  }
}

void SenderThread::serverConnect() {
  if (!m_calledDestructor) {
    checkObjects();
    emit connectSignal();
  }
}

void SenderThread::serverDisconnect() {
  if (!m_calledDestructor) {
    checkObjects();
    emit disconnectSignal();
  }
}

arguments * SenderThread::getArgs() {
  checkObjects();
  return sender->getArgs();
}

