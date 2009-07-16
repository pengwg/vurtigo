#include "GeomServerPlugin.h"

#include "GeomServerUI.h"

void GeomServerPlugin::objectModified(int objID) {
}

bool GeomServerPlugin::init() {
  senderThread.start();
  m_mainWidget = new GeomServerUI(senderThread);

  setUpdateTime(UPDATE_TIME);
  return true;
}

void GeomServerPlugin::cleanup() {
  // No more updates
  setUpdateTime(-1);
  (static_cast<GeomServerUI*>(m_mainWidget))->serverDisconnect();
  senderThread.quit();
  senderThread.wait();
}

void GeomServerPlugin::update() {
   senderThread.readAndSetData();
}

void GeomServerPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin.
Q_EXPORT_PLUGIN2(GeomServer, GeomServerPlugin)
