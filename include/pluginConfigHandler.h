#ifndef PLUGIN_CONFIG_HANDLER_H
#define PLUGIN_CONFIG_HANDLER_H

#include <QXmlDefaultHandler>
#include <QList>
#include <QString>

//! The info from a plugin extracted from XML
class PluginXmlInfo {
 public:
  QString title;
  QString version;
  QString libName;
  QString libPath;
};

//! XML handler for plugin config files.
class PluginConfigHandler : public QXmlDefaultHandler {
 public:
  //! Constructor
  PluginConfigHandler();
  //! Destructor
  ~PluginConfigHandler();

  //! Called by the reader when the document is started.
  bool startDocument();
  bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
  bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
  bool endDocument();

  PluginXmlInfo currentInfo;
  QList<PluginXmlInfo> pluginInfo;

};

#endif
