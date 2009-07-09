#include "DICOMReaderPlugin.h"
#include "DICOMReaderUI.h"
#include <iostream>

void DICOMReaderPlugin::objectModified(int objID) {

}

bool DICOMReaderPlugin::init() {
  m_mainWidget = new DICOMReaderUI();
  return true;
}

void DICOMReaderPlugin::cleanup() {

}

void DICOMReaderPlugin::update(){

}

void DICOMReaderPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(DICOMReader, DICOMReaderPlugin)
