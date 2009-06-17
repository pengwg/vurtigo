#include "GeomServerUI.h"
#include <QFileDialog>
#include <iostream>
#include <sstream>

#include "arguments.h"

using namespace std;

//! Setup slots for UI actions
void GeomServerUI::setupSlots() {
  connect(connectButton, SIGNAL(clicked()), this, SLOT(serverConnect()));
  connect(disconnectButton, SIGNAL(clicked()), this, SLOT(serverDisconnect()));
  connect(fileDirToolButton, SIGNAL(clicked()), this, SLOT(selectSendFile()));
  connect(sendFileButton, SIGNAL(clicked()), this, SLOT(sendFile()));
}

//! Set defaults of the UI state
void GeomServerUI::setupDefaults() {
  arguments * args = sender.getArgs();
  std::stringstream sstream;

  hostLineEdit->insert(args->hostname);
  sstream << args->port;
  portLineEdit->insert(sstream.str().c_str());
}

//! Actions on startup
void GeomServerUI::init() {
  if (connStartCheckBox->isChecked())
      serverConnect();
}

GeomServerUI::GeomServerUI(SenderSimp & sender) : sender(sender) {
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
  arguments * args = sender.getArgs();

  args->hostname = hostLineEdit->text().toAscii().data();

  portLineEdit->text().toInt(&ok);
  if (ok)
      args->port = portLineEdit->text().toInt();

  sender.connectAndMessage();
}

//! Disonnect from server
void GeomServerUI::serverDisconnect() {
  sender.disconnect();
}


//! Uses dialog box to put the file directory to the file directory label
void GeomServerUI::selectSendFile() {
  const char * message = "Select file to send";
  fileDirLineEdit->insert(QFileDialog::getOpenFileName(this, message, ".", OPEN_FILE_TYPES));
}

//! For text files, they may have internal file types. This function returns the file type relating to the text file from user input
SenderSimp::TxtFileType GeomServerUI::checkboxTxtFileType() {
  SenderSimp::TxtFileType fileType = SenderSimp::tft_None;

//  if (fileTypeCathRadio->isChecked()) {
//    fileType = SenderSimp::tft_Cath;
//  }

  return fileType;
}

//! Sends file to server
void GeomServerUI::sendFile() {
  QString fileDir = fileDirLineEdit->text();
  
  sender.sendFile(fileDir, checkboxTxtFileType());
}
