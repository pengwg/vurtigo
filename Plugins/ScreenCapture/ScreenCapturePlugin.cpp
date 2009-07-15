#include "ScreenCapturePlugin.h"
#include "ScreenCaptureUI.h"
#include <iostream>

void ScreenCapturePlugin::objectModified(int objID) {

}

bool ScreenCapturePlugin::init() {
  m_mainWidget = new ScreenCaptureUI();
  return true;
}

void ScreenCapturePlugin::cleanup() {

}

void ScreenCapturePlugin::update(){

}

void ScreenCapturePlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(ScreenCapture, ScreenCapturePlugin)
