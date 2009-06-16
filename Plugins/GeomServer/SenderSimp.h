#ifndef SENDER_SIMP_H
#define SENDER_SIMP_H
#include "sender.h"
#include "genericMode.h"
#include <QString>

#define DICOM_EXT ".dcm"
#define TXT_EXT ".txt"
#define OPEN_FILE_TYPES "*.dcm *.txt"

class SenderSimp {
  GeometrySender * sender;
  arguments args;
  GenericMode * readMode;
  GenericMode * writeMode;
  bool tempConnect;

  //! Sets default arguments for args
  void setSenderDefaults();

  public:
    enum TxtFileType { tft_None, tft_Cath, tft_Image };
    SenderSimp();
    ~SenderSimp();

    //! Connect and print appropriate message to console
    bool connectAndMessage();
    bool isConnected();
    void disconnect();

    void runReadMode();
    bool sendFile(QString & fileDir, TxtFileType fileType = tft_None);

    arguments * getArgs();
    std::vector<CATHDATA> & getCaths();

  private:
    bool setFileType(QString & fileDir, TxtFileType fileType = tft_None);
};

#endif
