#include "CathHistoryPlugin.h"
#include "CathHistoryUI.h"
#include <iostream>

void CathHistoryPlugin::objectModified(int objID) {

}

bool CathHistoryPlugin::init() {
  m_mainWidget = new CathHistoryUI();
  return true;
}

void CathHistoryPlugin::cleanup() {

}

void CathHistoryPlugin::update(){

}

void CathHistoryPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(CathHistory, CathHistoryPlugin)
