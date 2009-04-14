#include "rtPluginLoader.h"

rtPluginLoader::rtPluginLoader() {
  m_pluginHash.clear();


}

rtPluginLoader::~rtPluginLoader() {

}

//! Load plugins from the specified config file.
/*!
  \param file The plugin configuration file.
  \return True if at least one plugin was loaded from the file.
 */
bool loadPluginsFromConfig(QFile file) {
  
}

//! Return a plugin with a specific ID.
DataInterface* rtPluginLoader::getPluginWithID(int ID) {
  return NULL;
}

