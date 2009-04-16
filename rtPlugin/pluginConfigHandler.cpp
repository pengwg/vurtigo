#include "pluginConfigHandler.h"
#include <iostream>

PluginConfigHandler::PluginConfigHandler() {

}


PluginConfigHandler::~PluginConfigHandler() {

}

bool PluginConfigHandler::startDocument() {
  pluginInfo.clear();
  return true;
}

bool PluginConfigHandler::startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
  if (localName == "plugin") {
  } else if (localName == "name") {
    currentInfo.title = atts.value("title");
    currentInfo.version = atts.value("version");
  } else if (localName == "lib") {
    currentInfo.libName = atts.value("libname");
    currentInfo.libPath = atts.value("libpath");
  } else {
    std::cout << "Unrecognized tag: " << localName.toStdString() << std::endl;
  }
  return true;
}

bool PluginConfigHandler::endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {

  if (localName == "plugin") {
    pluginInfo.append(currentInfo);
  }

  return true;
}

bool PluginConfigHandler::endDocument() {
  return true;
}
