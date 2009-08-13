#include "GeomServerUI.h"
#include <QFileDialog>
#include <iostream>
#include <sstream>
#include "arguments.h"
#include "SenderSimp.h"

using namespace std;

//! Setup slots for UI actions
void GeomServerUI::setupSlots() {
  connect(connectButton, SIGNAL(clicked()), this, SLOT(serverConnect()));
  connect(disconnectButton, SIGNAL(clicked()), this, SLOT(serverDisconnect()));
}

//! Set defaults of the UI state
void GeomServerUI::setupDefaults() {
  arguments * args = senderThread.getArgs();
  std::stringstream sstream;

  hostLineEdit->insert(args->hostname);
  sstream << args->port;
  portLineEdit->insert(sstream.str().c_str());
}

//! Actions on startup
void GeomServerUI::init() {
}

GeomServerUI::GeomServerUI(SenderThread & senderThread) : senderThread(senderThread) {
  setupUi(this);
  setupDefaults();
  setupSlots();
  init();
}

GeomServerUI::~GeomServerUI() {
}

//! Connect to server
void GeomServerUI::serverConnect() {
  bool ok;
  arguments * args = senderThread.getArgs();

  SenderSimp::copyString(&args->hostname, hostLineEdit->text().toAscii().data());
  
  portLineEdit->text().toInt(&ok);
  if (ok)
      args->port = portLineEdit->text().toInt();

  senderThread.serverConnect();
}

//! Disonnect from server
void GeomServerUI::serverDisconnect() {
  senderThread.serverDisconnect();
}



