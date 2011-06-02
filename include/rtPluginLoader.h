#ifndef RT_PLUGIN_LOADER_H
#define RT_PLUGIN_LOADER_H

#include <QHash>
#include <QList>
#include <QFile>
#include <QXmlSimpleReader>
#include <QPluginLoader>
#include <QTreeWidgetItem>

#include "rtPluginInterface.h"

class PluginXmlInfo;

//! Tries to load all of the plugins.
/*!
  The plugins will be listed in an XML file. That file will be read and an attempt will be made to load each plugin.
  Plugins that cannot be loaded will be ignored and will not appear in the plugin list later.
 */
class rtPluginLoader {
 public:
  //! Constructor
  rtPluginLoader();

  //! Destructor
  ~rtPluginLoader();

  //! Load plugins from the specified config file.
/*!
  \param file The plugin configuration file.
  \return True if at least one plugin was loaded from the file.
 */
  bool loadPluginsFromConfig(QFile *file);

  //! Save plugins to the specified config file
  bool savePluginsToFile(QFile *file);

  //! Return a plugin with a specific ID.
  DataInterface* getPluginWithID(int ID);

  //! Add a plugin to the click watch list.
  bool addToClickWatch(int ID);

  //! Remove a plugin from the click watch list
  bool removeFromClickWatch(int ID);

  //! A point in the 3D view has been selected. Pass it to all the plugins in the click watch list.
  void point3DSelected(double px, double py, double pz, int intensity);

  //! Call update on each plugin
  /*!
    Each plugin can determine if it needs to call the update function. The update function will not be called on plugins where the required update time has not elapsed.
  */
  void updatePlugins();

  //! Called every time an object is modified.
  /*!
  This function may be called quite often since every object that changes will call this function.
  @param objId The object ID of the object that was modified.
  */
  void objectModified(int objId);

 protected:
  //! Return the next available unique ID.
  int getNextFreeID();

  QHash<int, QPluginLoader*> m_loaderHash;
  QHash<int, DataInterface*> m_pluginHash;
  QHash<int, QTreeWidgetItem*> m_widgetItemHash;
  QHash<int, PluginXmlInfo> m_pluginInfoHash;

  //! List of plugins that have registered for the click watch.
  QList<int> m_clickWatchList;

  //! XML reader when laoding plugins from file.
  QXmlSimpleReader m_xmlReader;

 private:
  //! Private copy-constructor
  rtPluginLoader(const rtPluginLoader&);
  //! Private equals operator
  rtPluginLoader& operator=(const rtPluginLoader&);
};

#endif
