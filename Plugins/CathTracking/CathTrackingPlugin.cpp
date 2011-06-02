#include "CathTrackingPlugin.h"
#include "CathTrackingUI.h"
#include <iostream>

void CathTrackingPlugin::objectModified(int objID) {

}

bool CathTrackingPlugin::init() {
  m_mainWidget = new CathTrackingUI();
  return true;
}

void CathTrackingPlugin::cleanup() {

}

void CathTrackingPlugin::update(){

}

void CathTrackingPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(CathTracking, CathTrackingPlugin)
