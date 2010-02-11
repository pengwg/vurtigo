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

// Vurtigo
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtMainWindow.h"
#include "rtPluginLoader.h"
#include "rtConfigOptions.h"
#include "rtTimeManager.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"
#include "buildParam.h"
#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"

// Qt
#include <QCoreApplication>
#include <QTextStream>

// C++
#include <sstream>

rtApplication::rtApplication() {
  // This defaults to standard out.
  m_stream = new QTextStream(stdout, QIODevice::WriteOnly);
  m_runVurtigo = true;

  m_pluginLoader = NULL;
  m_mainWindow = NULL;
  m_configOptions = NULL;
  m_timeManager = NULL;
  m_message = NULL;
  m_objManager = NULL;

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

void rtApplication::initApp(QStringList args) {
  // setup the messaging
  m_message = new rtMessage();
  m_message->setStream(m_stream);

  // Parse the command line arguments.
  parseArgs(args);

  if (m_runVurtigo) {
    m_configOptions = new rtConfigOptions();
    m_mainWindow = new rtMainWindow();
    m_pluginLoader = new rtPluginLoader();
    m_timeManager = new rtTimeManager();
    m_timeManager->startRenderTimer(40);

    m_objManager = new rtObjectManager();

    rtBaseHandle::instance(); // Important to create the object in THIS THREAD.

#ifdef DEBUG_VERBOSE_MODE_ON
      m_message->debug( QString("initApp() Show Window") );
#endif

    m_mainWindow->setupHelpFiles();
    m_mainWindow->show();

    setupOrigin();

    QList<QString> pluginList;
    pluginList = m_configOptions->getPluginList();

#ifdef DEBUG_VERBOSE_MODE_ON
      m_message->debug( QString("initApp() Processing Plugin List") );
#endif

    for (int ix1=0; ix1<pluginList.size(); ix1++) {
      QFile file(pluginList[ix1]);
      if (!file.exists()) {
        m_message->error(__LINE__, __FILE__, QString("File not found: ").append(pluginList[ix1]));
        continue;
      }
      if (!m_pluginLoader->loadPluginsFromConfig(&file)) {
        m_message->error(__LINE__, __FILE__, QString("Failed to load plugins from: ").append(pluginList[ix1]));
      }
    }
  }
}

void rtApplication::cleanupApp() {
  if(m_mainWindow) delete m_mainWindow;
  if(m_pluginLoader) delete m_pluginLoader;
  if(m_configOptions) delete m_configOptions;
  if(m_timeManager) delete m_timeManager;
  if(m_objManager) delete m_objManager;
  if(m_message) delete m_message;
}

/////////
// Protected Functions
////////
void rtApplication::parseArgs(QStringList args) {

  // Start at 1 since the first element is the name of the program
  for (int ix1=1; ix1<args.count(); ix1++) {
    if (args.at(ix1) == "--version" || args.at(ix1) == "-v") {
      std::stringstream msg;
      msg << "Vurtigo Version: " << VURTIGO_MAJOR_VERSION << "." << VURTIGO_MINOR_VERSION;
      m_message->log(msg.str());
      m_runVurtigo = false; // Just checking the version no need to run the software.
    }
  }

}


void rtApplication::setupOrigin() {

#ifdef DEBUG_VERBOSE_MODE_ON
      m_message->debug( QString("setupOrigin() Start") );
#endif

  // Setup the origin.
  rt3DPointBufferRenderObject *renObj = static_cast<rt3DPointBufferRenderObject*>(m_objManager->addObjectOfType(rtConstants::OT_3DPointBuffer, "True Origin"));
  rt3DPointBufferDataObject *datObj = static_cast<rt3DPointBufferDataObject*>(renObj->getDataObject());
  rt3DPointBufferDataObject::SimplePoint origin;
  rt3DPointBufferDataObject::SimplePoint xDir, yDir, zDir;
  origin.px = 0.0f;
  origin.py = 0.0f;
  origin.pz = 0.0f;
  origin.pSize = 5.0f;
  datObj->addPoint(origin);

  for (int ix1=10; ix1<=100; ix1+=10) {
    xDir.px = ix1;
    xDir.py = 0.0f;
    xDir.pz = 0.0f;
    xDir.pSize = 2.0f;
    datObj->addPoint(xDir);
    yDir.px = 0.0f;
    yDir.py = ix1;
    yDir.pz = 0.0f;
    yDir.pSize = 2.0f;
    datObj->addPoint(yDir);
    zDir.px = 0.0f;
    zDir.py = 0.0f;
    zDir.pz = ix1;
    zDir.pSize = 2.0f;
    datObj->addPoint(zDir);
  }
  datObj->Modified();

#ifdef DEBUG_VERBOSE_MODE_ON
      m_message->debug( QString("setupOrigin() End") );
#endif
}
