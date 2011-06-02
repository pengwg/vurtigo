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

