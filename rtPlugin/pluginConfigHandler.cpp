#include "pluginConfigHandler.h"

PluginConfigHandler::PluginConfigHandler() {

}


PluginConfigHandler::~PluginConfigHandler() {

}

bool PluginConfigHandler::startDocument () {
  return true;
}

bool PluginConfigHandler::startElement (const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
  return true;
}

bool PluginConfigHandler::endElement (const QString & namespaceURI, const QString & localName, const QString & qName) {
  return true;
}

bool PluginConfigHandler::endDocument () {
  return true;
}
