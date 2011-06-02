#ifndef SENDER_SIMP_H
#define SENDER_SIMP_H
#include "sender.h"
#include "genericMode.h"
#include <QString>
#include <QObject>
#include <QSemaphore>

//! A wrapping class for sending information
/*!
  Uses GeometrySender and GenericMode from geometryTester and simplifies it
  */
class SenderSimp : public QObject {
  Q_OBJECT

  public:
    SenderSimp();
    ~SenderSimp();
    bool isConnected();

    arguments * getArgs();
    std::vector<CATHDATA> & getCaths();
    std::vector<IMAGEDATA> & getImages();
    static void copyString(char ** const dest, const char * const src);

    GenericMode* getReaderHandle() { return readMode; }

    //! Check is a new signal can be sent.
    bool allocateNewSignal() { return m_signalsAvailable.tryAcquire(); }

   public slots:
    bool connectAndMessage();
    void disconnect();

    void print();
    void runReadMode();

signals:
    void isNowConnected(char *,int);

protected:
  //! Sends info, needed by Generic Mode
  GeometrySender sender;
  //! Config information for connecting
  arguments args;
  //! For reading information
  GenericMode * readMode;
  //! true, if the application is connecting temporarily (for sending information)
  bool tempConnect;

  //! Semaphore to ensure only one read process is running at a time.
  QSemaphore runLock;

  void setSenderDefaults();

  QSemaphore m_signalsAvailable;
};

#endif
