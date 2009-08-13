#include "SenderThread.h"
#include <QMetaType>

SenderThreadObject::SenderThreadObject() {
  queuedReads = 0;
}

SenderThreadObject::~SenderThreadObject() {
}

//! Read data from server and set internal vurtigo objects to match
void SenderThreadObject::readAndSetData() {
    if (sender.isConnected()) {
      sender.runReadMode();
      converter.setLocalCathAll(sender);
      converter.setLocalImageAll(sender);

//      // Print read values
//      sender.print();
//      // Print local values
//      converter.printHeaderFooter(&Converter::printAll);
    }
    queuedReads--;
}

SenderThread::SenderThread() {
  threadObj = NULL;
  calledDestructor = false;
}

SenderThread::~SenderThread() {
  calledDestructor = true;
  objLock.lock();
  delete threadObj;
  this->quit();
  while(!this->wait());
  objLock.unlock();
}

//! The thread actions, which connects the signals and slots for the new thread
void SenderThread::run() {
  threadObj = new SenderThreadObject();
  connect(this, SIGNAL(connectSignal()), &threadObj->sender, SLOT(connectAndMessage()), Qt::QueuedConnection);
  connect(this, SIGNAL(disconnectSignal()), &threadObj->sender, SLOT(disconnect()), Qt::QueuedConnection);
  connect(this, SIGNAL(readAndSetDataSignal()), threadObj, SLOT(readAndSetData()), Qt::QueuedConnection);
  objCV.wakeAll();
  exec();
}

//! Method to check if the "threadObj" is NULL before doing actions
void SenderThread::checkObjects() {
  if (threadObj == NULL) {
    objLock.lock();
    objCV.wait(&objLock);
    objLock.unlock();
  }
}

//! Read from Geometry Server and set data in Vurtigo
void SenderThread::readAndSetData() {
  if (threadObj->queuedReads < MAX_QUEUED_READS) {
    checkObjects();
    threadObj->queuedReads++;
    emit readAndSetDataSignal();
  }
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
  return threadObj->sender.getArgs();
}

