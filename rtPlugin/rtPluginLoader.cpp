#include "rtPluginLoader.h"
#include "pluginConfigHandler.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlInputSource>
#include <iostream>

rtPluginLoader::rtPluginLoader() {
  m_pluginHash.clear();
  m_loaderHash.clear();

}

rtPluginLoader::~rtPluginLoader() {
  QPluginLoader* tempLoader;
  QTreeWidgetItem* treeItem;
  QList<int> keys = m_loaderHash.uniqueKeys();
  int currKey;

  while(!m_loaderHash.empty()) {
    currKey = keys.takeFirst();
    tempLoader = m_loaderHash.take(currKey);

    treeItem = m_widgetItemHash.take(currKey);
    if(treeItem) delete treeItem;

    m_pluginHash.take(currKey)->cleanup();
    if (tempLoader) {
      tempLoader->unload();
      delete tempLoader;
    }
  }
}

//! Load plugins from the specified config file.
/*!
  \param file The plugin configuration file.
  \return True if at least one plugin was loaded from the file.
 */
bool rtPluginLoader::loadPluginsFromConfig(QFile* file) {
  bool ok = false;
  int ix1;
  int nextID;

  if (file->exists()) {
    QXmlInputSource source(file);
    PluginConfigHandler handler;
    QPluginLoader* tempLoader;
    DataInterface* tempPlugin;
    QTreeWidgetItem* tempTreeItem;
    QString fullName;

    m_xmlReader.setContentHandler(&handler);
    m_xmlReader.setErrorHandler(&handler);	

    if (m_xmlReader.parse(&source)) {
      for (ix1=0; ix1<handler.pluginInfo.size(); ix1++) {
        nextID = getNextFreeID();
        QDir appDir(qApp->applicationDirPath());
        QDir givenDir(handler.pluginInfo[ix1].libPath);

        // Format the plugin name according to the OS
#ifdef Q_OS_LINUX	
        fullName = handler.pluginInfo[ix1].libName+".so";
#endif
#ifdef Q_OS_WIN32
        fullName = handler.pluginInfo[ix1].libName+".dll";
#endif
#ifdef Q_OS_MAC
        fullName = handler.pluginInfo[ix1].libName+".dylib";
#endif

        if (!QFile::exists(appDir.absoluteFilePath(fullName)) && !QFile::exists(givenDir.absoluteFilePath(fullName))) {
          fullName = "lib"+fullName;
        }

        if ( QLibrary::isLibrary(appDir.absoluteFilePath(fullName)) ) {
          tempLoader = new QPluginLoader(appDir.absoluteFilePath(fullName));
        }
        else if ( QLibrary::isLibrary(givenDir.absoluteFilePath(fullName)) ) {
          tempLoader = new QPluginLoader(givenDir.absoluteFilePath(fullName));
        }
        else {
          tempLoader = NULL;
          std::cout << "No such library: " << fullName.toStdString() << std::endl;
        }

        if (tempLoader && tempLoader->load()) {
          tempPlugin = qobject_cast<DataInterface*>(tempLoader->instance());

          tempTreeItem = new QTreeWidgetItem();
          tempTreeItem->setText(0, QString::number(nextID));
          tempTreeItem->setText(1, handler.pluginInfo[ix1].title);
          tempTreeItem->setText(2, handler.pluginInfo[ix1].version);

          m_loaderHash.insert(nextID, tempLoader);
          m_pluginHash.insert(nextID, tempPlugin);
          m_widgetItemHash.insert(nextID, tempTreeItem);
          tempPlugin->setUniqueID(nextID);
          tempPlugin->init();

          rtObjectManager::instance().getMainWinHandle()->updatePluginList(&m_widgetItemHash);

          ok = true;
        } else if (tempLoader) {
          std::cout << "Plugin: " << tempLoader->fileName().toStdString() << " could not be loaded." << std::endl;
          std::cout << tempLoader->errorString().toStdString() << std::endl;
          delete tempLoader;
        }
      }
    } else {
      std::cout << "Error: rtPluginLoader.cpp: Could not parse File." << std::endl;
    }
  } else {
    std::cout << "Error: rtPluginLoader.cpp: No such file... " << std::endl;
  }

  return ok;
}

//! Return a plugin with a specific ID.
DataInterface* rtPluginLoader::getPluginWithID(int ID) {
  DataInterface* obj = NULL;
  if (m_pluginHash.contains(ID)) {
    obj=m_pluginHash.value(ID);
  }
  return obj;
}

// Add a plugin to the click watch list.
bool rtPluginLoader::addToClickWatch(int ID) {
  if (!m_clickWatchList.contains(ID) && m_pluginHash.contains(ID)) {
    m_clickWatchList.append(ID);
    return true;
  }
  return false;
}

//! Remove a plugin from the click watch list
bool rtPluginLoader::removeFromClickWatch(int ID) {
  if (m_clickWatchList.contains(ID)) {
    m_clickWatchList.removeAll(ID);
    return true;
  }
  return false;
}

//! A point in the 3D view has been selected. Pass it to all the plugins in the click watch list.
void rtPluginLoader::point3DSelected(double px, double py, double pz, int intensity) {
  int ix1;
  for (ix1=0; ix1<m_clickWatchList.size(); ix1++) {
    m_pluginHash.value(m_clickWatchList.value(ix1))->point3DSelected(px, py, pz, intensity);
  }
}

//! Call update on each plugin
/*!
  Each plugin can determine if it needs to call the update function. The update function will not be called on plugins where the required update time has not elapsed.
  */
void rtPluginLoader::updatePlugins() {
  QHashIterator<int, DataInterface*> i(m_pluginHash);
   while (i.hasNext()) {
     i.next();
     i.value()->tryUpdate();
   }
}

//! Called every time an object is modified.
/*!
  This function may be called quite often since every object that changes will call this function.
  @param objId The object ID of the object that was modified.
  */
void rtPluginLoader::objectModified(int objId) {
  QHashIterator<int, DataInterface*> i(m_pluginHash);
  while (i.hasNext()) {
    i.next();
    if ( i.value()->isInWatchList(objId) ) {
      i.value()->objectModified(objId);
    }
  }
}

//! Return the next available unique ID.
int rtPluginLoader::getNextFreeID() {
  int ix1;

  // Maximum number of plugins is 1000
  for (ix1=0; ix1<1000; ix1++) {
    if (!m_loaderHash.contains(ix1) && !m_pluginHash.contains(ix1)) {
      return ix1;
    }
  }

  std::cout << "Error: Max Number of Plugins Exceeded." << std::endl;
  return -1;
}
