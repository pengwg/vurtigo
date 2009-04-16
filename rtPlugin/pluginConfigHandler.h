#ifndef PLUGIN_CONFIG_HANDLER_H
#define PLUGIN_CONFIG_HANDLER_H

#include <QXmlDefaultHandler>
#include <QList>
#include <QString>

class PluginXmlInfo {
 public:
  QString title;
  QString version;
  QString libName;
  QString libPath;
};

class PluginConfigHandler : public QXmlDefaultHandler {
 public:
  PluginConfigHandler();
  ~PluginConfigHandler();

  bool startDocument();
  bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
  bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
  bool endDocument();

  PluginXmlInfo currentInfo;
  QList<PluginXmlInfo> pluginInfo;

};

#endif
