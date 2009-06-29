#include "TestSuitePlugin.h"
#include <iostream>

void TestSuitePlugin::objectModified(int objID) {

}

bool TestSuitePlugin::init() {
  m_mainWidget = new TestSuiteUI();
  return true;
}

void TestSuitePlugin::cleanup() {

}

void TestSuitePlugin::update(){

}

void TestSuitePlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(TestSuite, TestSuitePlugin)
