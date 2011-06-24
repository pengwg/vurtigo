/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "rtPluginLoader.h"
#include "pluginConfigHandler.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

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
      tempLoader->deleteLater();
    }
  }
}

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

    bool alreadyExists;

    if (m_xmlReader.parse(&source)) {
      for (ix1=0; ix1<handler.pluginInfo.size(); ix1++) {

          QHashIterator<int,PluginXmlInfo> itr(m_pluginInfoHash);

          alreadyExists = false;
          while (itr.hasNext())
          {
              itr.next();
              // if that plugin is already loaded
              if ( (itr.value().title == handler.pluginInfo[ix1].title) )
              {
                  alreadyExists = true;
                  break;
              }
          }
          if (alreadyExists) continue;

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

        if ( QFile::exists(appDir.absoluteFilePath(fullName)) && QLibrary::isLibrary(appDir.absoluteFilePath(fullName)) ) {
          tempLoader = new QPluginLoader(appDir.absoluteFilePath(fullName));
        }
        else if ( QFile::exists(givenDir.absoluteFilePath(fullName)) && QLibrary::isLibrary(givenDir.absoluteFilePath(fullName)) ) {
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

          PluginXmlInfo tmpInfo;
          tmpInfo.title = handler.pluginInfo[ix1].title;
          tmpInfo.version = handler.pluginInfo[ix1].version;
          tmpInfo.libName = handler.pluginInfo[ix1].libName;
          tmpInfo.libPath = handler.pluginInfo[ix1].libPath;
          m_pluginInfoHash.insert(nextID,tmpInfo);

          m_loaderHash.insert(nextID, tempLoader);
          m_pluginHash.insert(nextID, tempPlugin);
          m_widgetItemHash.insert(nextID, tempTreeItem);
          tempPlugin->setUniqueID(nextID);
          tempPlugin->init();

          rtApplication::instance().getMainWinHandle()->updatePluginList(&m_widgetItemHash);

          ok = true;
        } else if (tempLoader) {
          std::cout << "Plugin: " << tempLoader->fileName().toStdString() << "(" << handler.pluginInfo[ix1].libName.toStdString() << ") could not be loaded." << std::endl;
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

bool rtPluginLoader::savePluginsToFile(QFile *file)
{
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
      return false;

    QXmlStreamWriter writer(file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("PluginList");

    QHashIterator<int, PluginXmlInfo> itr(m_pluginInfoHash);
    while (itr.hasNext())
    {
        itr.next();
        writer.writeStartElement("plugin");
        writer.writeStartElement("name");
        writer.writeAttribute("title",itr.value().title);
        writer.writeAttribute("version",itr.value().version);
        writer.writeEndElement();

        writer.writeStartElement("lib");
        writer.writeAttribute("libname",itr.value().libName);
        writer.writeAttribute("libpath",itr.value().libPath);
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    file->close();
    return true;
}

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

bool rtPluginLoader::removeFromClickWatch(int ID) {
  if (m_clickWatchList.contains(ID)) {
    m_clickWatchList.removeAll(ID);
    return true;
  }
  return false;
}

void rtPluginLoader::point3DSelected(double px, double py, double pz, int intensity) {
  int ix1;
  for (ix1=0; ix1<m_clickWatchList.size(); ix1++) {
    m_pluginHash.value(m_clickWatchList.value(ix1))->point3DSelected(px, py, pz, intensity);
  }
}


void rtPluginLoader::updatePlugins() {
  QHashIterator<int, DataInterface*> i(m_pluginHash);
   while (i.hasNext()) {
     i.next();
     i.value()->tryUpdate();
   }
}

void rtPluginLoader::objectModified(int objId) {
  QHashIterator<int, DataInterface*> i(m_pluginHash);
  while (i.hasNext()) {
    i.next();
    if ( i.value()->isInWatchList(objId) ) {
      i.value()->objectModified(objId);
    }
  }
}

////////////////
// Protected
////////////////
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
