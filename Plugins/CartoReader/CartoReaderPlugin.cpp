#include "CartoReaderPlugin.h"
#include "CartoReaderUI.h"
#include <iostream>

void CartoReaderPlugin::objectModified(int objID) {

}

bool CartoReaderPlugin::init() {
  m_mainWidget = new CartoReaderUI();
  return true;
}

void CartoReaderPlugin::cleanup() {

}

void CartoReaderPlugin::update(){

}

void CartoReaderPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(CartoReader, CartoReaderPlugin)
