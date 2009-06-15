#ifndef SENDER_SIMP_H
#define SENDER_SIMP_H
#include "sender.h"
#include "genericMode.h"
#include "readOnlyMode.h"

class SenderSimp {
  GeometrySender * sender;
  arguments args;
  GenericMode * readMode;

  //! Sets default arguments for args
  void setSenderDefaults();

  public:
    SenderSimp();
    ~SenderSimp();

    //! Connect and print appropriate message to console
    bool connectAndMessage();
    bool isConnected();
    void disconnect();

    void runReadMode();

    arguments * getArgs();
    std::vector<CATHDATA> & getCaths();
};

#endif
