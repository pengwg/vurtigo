#ifndef SENDER_THREAD_H
#define SENDER_THREAD_H

#include <QThread>
#include "Converter.h"
#include "SenderSimp.h"
#include <QMutex>
#include <QWaitCondition>

//Max number of threads queued for SenderThreadObject::readAndSetData()
#define MAX_QUEUED_READS    1

class SenderThread;

//! Class to help modulize actions done by the thread
class SenderThreadObject : public QObject {
  Q_OBJECT
  friend class SenderThread;

  unsigned int queuedReads;

  //! Helps map between remote and local data
  Converter converter;
  //! To communicate geometry server
  SenderSimp sender;

  SenderThreadObject();
  ~SenderThreadObject();

  private slots:
    void readAndSetData();
};

//! Thread class to work with a server on a different thread
class SenderThread : public QThread {
  Q_OBJECT

//! Holds objects used by thread
SenderThreadObject * threadObj;
// Ensures that "threadObj" above is not null when actions are done
QMutex objLock;
QWaitCondition objCV;
void checkObjects();

//! true when destructor is called, helps ensure that the thread is not running when being destroyed
bool calledDestructor;

public:
  SenderThread();
  ~SenderThread();
  void run();

  // Sender actions
  void readAndSetData();
  void serverConnect();
  void serverDisconnect();

  arguments * getArgs();

signals:
  // Sender signals for multithreading
  void readAndSetDataSignal();
  void connectSignal();
  void disconnectSignal();
};

#endif
