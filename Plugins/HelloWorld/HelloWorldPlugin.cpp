#include "HelloWorldPlugin.h"
#include "HelloWorldPluginUI.h"
#include <iostream>

void HelloWorldPlugin::objectModified(int objID) {

}

bool HelloWorldPlugin::init() {
    m_mainWidget = new HelloWorldPluginUI();
    return true;
}

void HelloWorldPlugin::cleanup() {
  std::cout << "ByeBye World! " << std::endl;
}

void HelloWorldPlugin::update(){

}

void HelloWorldPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(HelloWorld, HelloWorldPlugin)
