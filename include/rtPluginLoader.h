/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RT_PLUGIN_LOADER_H
#define RT_PLUGIN_LOADER_H

#include <QHash>
#include <QList>
#include <QFile>
#include <QXmlSimpleReader>
#include <QPluginLoader>
#include <QTreeWidgetItem>

#include "rtPluginInterface.h"



//! Tries to load all of the plugins.
/*!
  The plugins will be listed in an XML file. That file will be read and an attempt will be made to load each plugin.
  Plugins that cannot be loaded will be ignored and will not appear in the plugin list later.
 */
class rtPluginLoader {
 public:
  rtPluginLoader();
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
  //! Private copy-constructor
  rtPluginLoader(const rtPluginLoader&);
  rtPluginLoader& operator=(const rtPluginLoader&);
};

#endif
