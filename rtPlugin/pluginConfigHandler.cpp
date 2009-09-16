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
#include "pluginConfigHandler.h"
#include <iostream>

PluginConfigHandler::PluginConfigHandler() {

}


PluginConfigHandler::~PluginConfigHandler() {

}

bool PluginConfigHandler::startDocument() {
  pluginInfo.clear();
  return true;
}

bool PluginConfigHandler::startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
  if (localName == "plugin") {
  } else if (localName == "PluginList") {
  }else if (localName == "name") {
    currentInfo.title = atts.value("title");
    currentInfo.version = atts.value("version");
  } else if (localName == "lib") {
    currentInfo.libName = atts.value("libname");
    currentInfo.libPath = atts.value("libpath");
  } else {
    std::cout << "Unrecognized tag: " << localName.toStdString() << std::endl;
  }
  return true;
}

bool PluginConfigHandler::endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
  if (localName == "plugin") {
    pluginInfo.append(currentInfo);
  }

  return true;
}

bool PluginConfigHandler::endDocument() {
  return true;
}
