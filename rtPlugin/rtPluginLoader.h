#ifndef RT_PLUGIN_LOADER_H
#define RT_PLUGIN_LOADER_H

#include <QHash>
#include <QList>
#include <QFile>
#include <QXmlSimpleReader>
#include <QPluginLoader>
#include <QTreeWidgetItem>

#include "rtPluginInterface.h"



//! Tries to load all of the plugins. [Singleton]
/*!
  The plugins will be listed in an XML file. That file will be read and an attempt will be made to load each plugin. Plugins that cannot be loaded will be ignored and will not appear in the plugin list later. 
  This is a singleton class so to obtain access to this class use the instance() function. 
 */
class rtPluginLoader {
 public:

  //! Get the instance of this class.
  /*! 
    There should only be one instance of a class that does loading of plugins. Therefore, this is a singleton class. This function will always return the same instance of this class.
  */
  static rtPluginLoader& instance()
  {
    static rtPluginLoader loader;
    return loader;
  }

  ~rtPluginLoader();

  bool loadPluginsFromConfig(QFile *file);
  DataInterface* getPluginWithID(int ID);

  bool addToClickWatch(int ID);
  bool removeFromClickWatch(int ID);
  void point3DSelected(double px, double py, double pz, int intensity);

  void updatePlugins();

  void objectModified(int objId);

 protected:
  QHash<int, QPluginLoader*> m_loaderHash;
  QHash<int, DataInterface*> m_pluginHash;
  QHash<int, QTreeWidgetItem*> m_widgetItemHash;

  QList<int> m_clickWatchList;

  QXmlSimpleReader m_xmlReader;

  int getNextFreeID();

 private:
  //! Private constructor since this is a singleton.
  rtPluginLoader();
  //! Private copy-constructor
  rtPluginLoader(const rtPluginLoader&);
  rtPluginLoader& operator=(const rtPluginLoader&);
};

#endif
