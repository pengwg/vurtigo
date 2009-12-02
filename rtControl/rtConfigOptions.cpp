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
#include <QXmlStreamReader>

#include "rtConfigOptions.h"
#include "buildParam.h"

rtConfigOptions::rtConfigOptions() {
  m_fileName = "";
  m_wlFileName = "";
  m_helpCollectionFile = "";
}


rtConfigOptions::~rtConfigOptions() {
}


bool rtConfigOptions::loadFile(QString fName) {
  bool result = false;
  QFile f(fName);
  m_fileName = fName;
  if (f.exists()) {
    if (f.open(QIODevice::ReadOnly)) {
      // Load the file in here.
      QXmlStreamReader xml(&f);
      QXmlStreamAttributes attribs;

      while (!xml.atEnd()) {
        xml.readNext();
        if ( xml.isStartElement() ) {
          attribs = xml.attributes();
          if (xml.name() == "WLFile") {
            QString path = attribs.value("path").toString();
            if (path == "DEFAULT") path = CONFIG_FILE_PATH;
            m_wlFileName = path.append(attribs.value("name"));
          } else if (xml.name() == "HelpCollection") {
           QString path = attribs.value("path").toString();
            if (path == "DEFAULT") path = CONFIG_FILE_PATH;
            m_helpCollectionFile = path.append(attribs.value("name"));
          }
        }
      }

      // Check for an error
      if (xml.hasError()) {
        result = false;
      } else {
        result = true;
      }

      f.close();
    }
  }

  if (!result) {
    m_fileName = "";
  }

  return result;
}


QString rtConfigOptions::getConfigFileName() {
  return m_fileName;
}

QString rtConfigOptions::getWLFileName() {
  return m_wlFileName;
}

QString rtConfigOptions::getHelpCollectionFileName() {
  return m_helpCollectionFile;
}
