#include "SenderSimp.h"
#include <iostream>
#include "readOnlyMode.h"
#include "writeOnlyMode.h"

using namespace std;

void SenderSimp::setSenderDefaults() {
  args.hostname = (char*)"panoptes";
  args.port = 1777;
  args.swap = false;
  args.iterations = 1;
  args.sleep = 0;
  args.outputImages = false;
  args.interactive = false;
  args.cathFile = NULL;
  args.transFile = NULL;
  args.volFile = NULL;
  args.userPrompt = false;
  args.setAllFile = NULL;
  args.dicomFile = NULL;
  args.movieFile = NULL;
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

bool SenderSimp::isConnected() {
  return sender->isConnected() && !tempConnect;
}

void SenderSimp::disconnect() {
  if (sender->isConnected()) {
    sender->disconnect();
    cout << "Disconnected." << endl;
  }
  else {
    cout << "Not connected." << endl;
  }
}

void SenderSimp::runReadMode() {
    readMode->runMode();
}

arguments * SenderSimp::getArgs() {
  return &args;
}

vector<CATHDATA> & SenderSimp::getCaths() {
  return readMode->getCaths();
}

bool SenderSimp::setFileType(QString & fileDir, TxtFileType fileType) {
  if (fileDir.endsWith(TXT_EXT)) {
    if (fileType == tft_Cath) {
      args.cathFile = fileDir.toAscii().data();
      return true;
    }
  }
  cout << "Send file type not supported. Check text file type?" << endl;
  return false;
}

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
