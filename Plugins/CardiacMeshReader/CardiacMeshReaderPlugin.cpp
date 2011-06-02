#include "CardiacMeshReaderPlugin.h"
#include "CardiacMeshReaderUI.h"
#include <iostream>

void CardiacMeshReaderPlugin::objectModified(int objID) {

}

bool CardiacMeshReaderPlugin::init() {
  m_mainWidget = new CardiacMeshReaderUI();
  return true;
}

void CardiacMeshReaderPlugin::cleanup() {

}

void CardiacMeshReaderPlugin::update(){

}

void CardiacMeshReaderPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(CardiacMeshReader, CardiacMeshReaderPlugin)
