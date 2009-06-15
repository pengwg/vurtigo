#include "GeomServerUI.h"
#include <QFileDialog>
#include <sstream>

#include "arguments.h"

void GeomServerUI::setupSlots() {
  connect(connectButton, SIGNAL(clicked()), this, SLOT(serverConnect()));
  connect(disconnectButton, SIGNAL(clicked()), this, SLOT(serverDisconnect()));
  connect(sendFileToolButton, SIGNAL(clicked()), this, SLOT(selectSendFile()));
  connect(connectButton, SIGNAL(clicked()), this, SLOT(sendFile()));
}

void GeomServerUI::setupDefaults() {
  arguments * args = sender.getArgs();
  std::stringstream sstream;

  hostLineEdit->insert(args->hostname);
  sstream << args->port;
  portLineEdit->insert(sstream.str().c_str());
}

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

void GeomServerUI::serverConnect() {
  bool ok;
  arguments * args = sender.getArgs();

  args->hostname = hostLineEdit->text().toAscii().data();

  portLineEdit->text().toInt(&ok);
  if (ok)
      args->port = portLineEdit->text().toInt();

  sender.connectAndMessage();
}

void GeomServerUI::serverDisconnect() {
  sender.disconnect();
}

void GeomServerUI::selectSendFile() {
  const char * message = "Select file to send";
  sendFileLineEdit->insert(QFileDialog::getOpenFileName(this, message, ".", OPEN_FILE_TYPES));
}

void GeomServerUI::sendFile() {

}
