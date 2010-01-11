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
#include <QFile>
#include <QDir>
#include <QXmlStreamReader>

#include <iostream>

#include "rtConfigOptions.h"
#include "buildParam.h"

rtConfigOptions::rtConfigOptions() {
  m_settings = new QSettings("Sunnybrook", "Vurtigo");

  m_configPath = QDir::homePath()+"/vurtigoConfig/";

  if (!m_settings->contains("WLFileName")) {
    m_settings->setValue("WLFileName", m_configPath+"WindowLevel.xml");
  }

  if (!m_settings->contains("SourceCodeHelp")) {
    m_settings->setValue("SourceCodeHelp", m_configPath+"VurtigoSrcHelp.qhc");
  }

  if (!m_settings->contains("VurtigoHelp")) {
    m_settings->setValue("VurtigoHelp", m_configPath+"VurtigoHelp.qhc");
  }

  if (!m_settings->contains("PluginList")) {
    QList<QVariant> temp;
    temp.append(QVariant(m_configPath+"DefPlugins.xml"));
    m_settings->setValue("PluginList", temp);
  }

}


rtConfigOptions::~rtConfigOptions() {
  if(m_settings) delete m_settings;
}

  rtConfigOptions& rtConfigOptions::instance() {
    static rtConfigOptions handle;
    return handle;
  }

QString rtConfigOptions::getWLFileName() {
  return m_settings->value("WLFileName").toString();
}

QString rtConfigOptions::getSourceCodeHelpFileName() {
  return m_settings->value("SourceCodeHelp").toString();
}

QString rtConfigOptions::getHelpCollectionFileName() {
  return m_settings->value("VurtigoHelp").toString();
}

QList<QString> rtConfigOptions::getPluginList() {
  QList<QVariant> val;
  QList<QString> result;
  val = m_settings->value("PluginList").toList();
  for (int ix1=0; ix1<val.size(); ix1++) {
    result.append(val[ix1].toString());
  }
  return result;
}
