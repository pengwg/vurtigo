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
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtMainWindow.h"
#include "rtPluginLoader.h"
#include "rtConfigOptions.h"

#include <QCoreApplication>

#include <QTextStream>

rtApplication::rtApplication() {
  // This defaults to standard out.
  m_stream = new QTextStream(stdout, QIODevice::WriteOnly);
  m_pluginLoader = NULL;
  m_mainWindow = NULL;
  m_configOptions = NULL;

  connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(cleanupApp()));
}

rtApplication::~rtApplication() {
  if(m_stream) delete m_stream;
}

rtApplication& rtApplication::instance() {
  static rtApplication handle;
  return handle;
}

void rtApplication::displayCopyright() {
  (*m_stream) << "Vurtigo Copyright (C) 2009 Sunnybrook Health Sciences Centre. ";
  (*m_stream) << "This program comes with ABSOLUTELY NO WARRANTY; ";
  (*m_stream) << "This is free software, and you are welcome to redistribute it under certain conditions; ";
  (*m_stream) << "See COPYING and COPYING.LESSER for details.\n\n";
  m_stream->flush();
}

void rtApplication::initApp() {
  // setup the messaging
  rtMessage::instance().setStream(m_stream);
  m_configOptions = new rtConfigOptions();
  m_mainWindow = new rtMainWindow();
  m_pluginLoader = new rtPluginLoader();
}

void rtApplication::cleanupApp() {
  if(m_mainWindow) delete m_mainWindow;
  if(m_pluginLoader) delete m_pluginLoader;
  if(m_configOptions) delete m_configOptions;
}
