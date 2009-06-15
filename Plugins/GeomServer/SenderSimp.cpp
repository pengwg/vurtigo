#include "SenderSimp.h"
#include <iostream>

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

  setSenderDefaults();
  readMode->init(sender, &args);
}

SenderSimp::~SenderSimp() {
  delete sender;
  delete readMode;
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
  return sender->isConnected();
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

//TODO: Assume GUI triggers this function? (yes, signals and slots QT)
bool sendInfo(/*info parm*/) {
  //change param to sendable obj

  //TODO: Assume send a rtObjectType::OT_Cath or ImageData?? (yes these two)
  //return true or false

//  if (sender->isConnected()) {
//    converter->setLocalCathAll(*mode);
//  }
//  else if (connectAndMessage(sender)) {
//    converter->setLocalCathAll(*mode);
//  }
}
