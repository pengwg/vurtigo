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
