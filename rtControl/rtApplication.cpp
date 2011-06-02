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
#include "rtBasic3DPointData.h"

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
  rt3DPointBufferRenderObject *renObj = static_cast<rt3DPointBufferRenderObject*>(m_objManager->addObjectOfType("OT_3DPointBuffer", "True Origin"));
  rt3DPointBufferDataObject *datObj = static_cast<rt3DPointBufferDataObject*>(renObj->getDataObject());
  rtBasic3DPointData origin;
  rtBasic3DPointData xDir, yDir, zDir;
  origin.setX(0.0f);
  origin.setY(0.0f);
  origin.setZ(0.0f);
  origin.setPointSize(5.0f);
  datObj->addPoint(origin);

  for (int ix1=10; ix1<=100; ix1+=10) {
    xDir.setX(ix1);
    xDir.setY(0.0f);
    xDir.setZ(0.0f);
    xDir.setPointSize(2.0f);
    datObj->addPoint(xDir);
    yDir.setX(0.0f);
    yDir.setY(ix1);
    yDir.setZ(0.0f);
    yDir.setPointSize(2.0f);
    datObj->addPoint(yDir);
    zDir.setX(0.0f);
    zDir.setY(0.0f);
    zDir.setZ(ix1);
    zDir.setPointSize(2.0f);
    datObj->addPoint(zDir);
  }
  datObj->Modified();

#ifdef DEBUG_VERBOSE_MODE_ON
      m_message->debug( QString("setupOrigin() End") );
#endif
}
