#ifndef SENDER_SIMP_H
#define SENDER_SIMP_H
#include "sender.h"
#include "genericMode.h"
#include <QString>

//Definitions for filetypes
#define DICOM_EXT ".dcm"
#define TXT_EXT ".txt"
#define OPEN_FILE_TYPES "*.dcm *.txt"

//! A wrapping class for sending information
/*!
  Uses GeometrySender and GenericMode from geometryTester and simplifies it
  */
class SenderSimp {
  //! Sends info, needed by Generic Mode
  GeometrySender * sender;
  //! Config information for connecting
  arguments args;
  //! For reading information
  GenericMode * readMode;
  //! For writing information
  GenericMode * writeMode;
  //! true, if the application is connecting temporarily (for sending information)
  bool tempConnect;

  void setSenderDefaults();

  public:
    //! File types for text files (currenly not used)
    enum TxtFileType { tft_None };
    SenderSimp();
    ~SenderSimp();

    bool connectAndMessage();
    bool isConnected();
    void disconnect();

    void runReadMode();
    bool sendFile(QString & fileDir, TxtFileType fileType = tft_None);

    arguments * getArgs();
    std::vector<CATHDATA> & getCaths();
    std::vector<IMAGEDATA> & getImages();

  private:
    bool setFileType(QString & fileDir, TxtFileType fileType = tft_None);
};

#endif
