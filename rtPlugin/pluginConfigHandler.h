#ifndef PLUGIN_CONFIG_HANDLER_H
#define PLUGIN_CONFIG_HANDLER_H

#include <QXmlDefaultHandler>

class PluginConfigHandler : public QXmlDefaultHandler {
 public:
  PluginConfigHandler();
  ~PluginConfigHandler();

  bool startDocument();
  bool startElement (const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
  bool endElement (const QString & namespaceURI, const QString & localName, const QString & qName);
  bool endDocument ();
};

#endif
