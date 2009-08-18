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
  void run();

  // Sender actions
  void readAndSetData();
  void serverConnect();
  void serverDisconnect();

  arguments * getArgs();

  //! Get the handle to the sender object.
  SenderSimp* getSender() { return sender; }

protected:
  //! To communicate geometry server
  SenderSimp *sender;

  // Ensures that "threadObj" above is not null when actions are done
  QMutex objLock;
  QWaitCondition objCV;
  void checkObjects();

  //! true when destructor is called, helps ensure that the thread is not running when being destroyed
  bool calledDestructor;

signals:
  // Sender signals for multithreading
  void readAndSetDataSignal();
  void connectSignal();
  void disconnectSignal();
};

#endif
