#include "SenderSimp.h"
#include <iostream>
#include "readOnlyMode.h"
#include "writeOnlyMode.h"

using namespace std;

//! Sets default arguments for args
void SenderSimp::setSenderDefaults() {
  args.hostname = (char*)"panoptes";
  args.port = 1777;
  args.swap = false;
  args.cathFile = NULL;
  args.dicomFile = NULL;
}

SenderSimp::SenderSimp() {
  sender = new GeometrySender();
  readMode = new ReadOnlyMode();
  writeMode = new WriteOnlyMode();
  tempConnect = false;

  setSenderDefaults();
  readMode->init(sender, &args);
}

SenderSimp::~SenderSimp() {
  delete sender;
  delete readMode;
  delete writeMode;
}

//! Connect and print appropriate message to console
bool SenderSimp::connectAndMessage() {
  if (isConnected()) {
    cout << "Already connected." << endl;
    return true;
  }
  else if (!sender->connect(args.hostname,args.port,args.swap)) {
    char * server = "Geometry Server";
    cerr << "Error connecting to " << server <<". Check that " << server << " is running on port " << args.port << " on host: " << args.hostname << endl;
    sender->disconnect();
    return false;
  } else {
    cout << "Connection with: " << args.hostname << ":" << args.port << " established." << endl;
    return true;
  }
}


//! Returns true if the sender is connected permanently (not temporarily)
bool SenderSimp::isConnected() {
  return sender->isConnected() && !tempConnect;
}

//! Disconnects from the server
void SenderSimp::disconnect() {
  if (sender->isConnected()) {
    sender->disconnect();
    cout << "Disconnected." << endl;
  }
  else {
    cout << "Not connected." << endl;
  }
}

//! Reads the information
void SenderSimp::runReadMode() {
    readMode->runMode();
    readMode->print();
}

//! Returns the config information
arguments * SenderSimp::getArgs() {
  return &args;
}

//! Returns a vector of remote cathater objects
vector<CATHDATA> & SenderSimp::getCaths() {
  return readMode->getCaths();
}

//! Returns a vector of remote image objects
vector<IMAGEDATA> & SenderSimp::getImages() {
  return readMode->getImages();
}

//! Given a file directory and a text file type, this method sets the filename for "args" and returns true if it is a valid file type
bool SenderSimp::setFileType(QString & fileDir, TxtFileType fileType) {
  args.cathFile = NULL;
  args.dicomFile = NULL;

  if (fileDir.endsWith(TXT_EXT)) {
//Sample code for working with different text fle type
//    if (fileType == tft_Cath) {
//      args.cathFile = fileDir.toAscii().data();
//      return true;
//    }
    args.cathFile = fileDir.toAscii().data();
    return true;
  }
  else if (fileDir.endsWith(DICOM_EXT)) {
    args.dicomFile = fileDir.toAscii().data();
    return true;
  }
  cout << "Send file type not supported. Check text file type?" << endl;
  return false;
}

//! Sends a file, returns true if the file directory and file type is proper
/*!
  Will temporarily connect to the server if needed.
  @param fileDir file directory
  @param fileType file type of text file (not needed)
  @return true, if the file directory and file type is proper
 */
bool SenderSimp::sendFile(QString & fileDir, TxtFileType fileType) {
  if (setFileType(fileDir, fileType)) {
    if (!sender->isConnected()) {
      tempConnect = true;
      connectAndMessage();
    }

    writeMode->init(sender, &args);
    writeMode->runMode();

    if (tempConnect) {
      disconnect();
      tempConnect = false;
    }
    return true;
  }
  return false;
}
