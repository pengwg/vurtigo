#include "GeomServerPlugin.h"

#include "GeomServerUI.h"

void GeomServerPlugin::objectModified(int objID) {
}

bool GeomServerPlugin::init() {
  converter = new Converter();
  sender = new SenderSimp();

  m_mainWidget = new GeomServerUI(*sender);

  setUpdateTime(UPDATE_TIME);
  return true;
}

void GeomServerPlugin::cleanup() {
  delete converter;
  delete sender;
}

void GeomServerPlugin::update() {
    retrieveInfo();
}

void GeomServerPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

//! Get info from the server if connected
void GeomServerPlugin::retrieveInfo() {
  if (sender->isConnected()) {
    sender->runReadMode();
    converter->setLocalCathAll(*sender);
    converter->setLocalImageAll(*sender);
    converter->printHeaderFooter(&Converter::printAll);
  }
}

// This last line will export the plugin.
Q_EXPORT_PLUGIN2(GeomServer, GeomServerPlugin)
