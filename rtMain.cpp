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

/*
 File Created By: Stefan Pintilie
 File Edited By: Stefan Pintilie
 */

#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

#include <iostream> 
#include <sstream>
#include <cstdio>

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
#include "rtBaseHandle.h"
#include "rtMessage.h"
#include "rtConfigOptions.h"

#include "buildParam.h"

#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"

void setupOrigin();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    QStringList args = app.arguments();
    bool runVurtigo = true;
    int exitCode = 0;

    // Setup the message stream.
    // This defaults to standard out.
    QTextStream stream(stdout, QIODevice::WriteOnly);
    rtMessage::instance().setStream(&stream);

    stream << "Vurtigo Copyright (C) 2009 Sunnybrook Health Sciences Centre. ";
    stream << "This program comes with ABSOLUTELY NO WARRANTY; ";
    stream << "This is free software, and you are welcome to redistribute it under certain conditions; ";
    stream << "See COPYING and COPYING.LESSER for details.\n\n";
    stream.flush();

    // Start at 1 since the first element is the name of the program
    for (int ix1=1; ix1<args.count(); ix1++) {
      if (args.at(ix1) == "--version" || args.at(ix1) == "-v") {
        std::stringstream msg;
        msg << "Vurtigo Version: " << VURTIGO_MAJOR_VERSION << "." << VURTIGO_MINOR_VERSION;
        rtMessage::instance().log(msg.str());
        runVurtigo = false; // Just checking the version no need to run the software.
      }
    }

    // Check if the user wants Vurtigo to run and if the config file was loaded properly.
    if (runVurtigo) {
      rtConfigOptions::instance(); // Reead the config file before anything...
      rtTimeManager::instance().startRenderTimer(&mainWin, 40);
      rtObjectManager::instance().setMainWinHandle(&mainWin);
      rtBaseHandle::instance(); // Important to create the object in THIS THREAD.
      mainWin.setupHelpFiles();

      // Load default plugins
      QList<QString> pluginList;
      pluginList = rtConfigOptions::instance().getPluginList();
      for (int ix1=0; ix1<pluginList.size(); ix1++) {
        QFile file(pluginList[ix1]);
        if (!file.exists()) {
          rtMessage::instance().error(__LINE__, __FILE__, QString("File not found: ").append(pluginList[ix1]));
          continue;
        }
        if (!rtPluginLoader::instance().loadPluginsFromConfig(&file)) {
          rtMessage::instance().error(__LINE__, __FILE__, QString("Failed to load plugins from: ").append(pluginList[ix1]));
        }
      }

      setupOrigin();

      mainWin.show();
      exitCode = app.exec();
    }
    return exitCode;
}

void setupOrigin() {
  // Setup the origin.
  rt3DPointBufferRenderObject *renObj = static_cast<rt3DPointBufferRenderObject*>(rtObjectManager::instance().addObjectOfType(rtConstants::OT_3DPointBuffer, "True Origin"));
  rt3DPointBufferDataObject *datObj = static_cast<rt3DPointBufferDataObject*>(renObj->getDataObject());
  rt3DPointBufferDataObject::SimplePoint origin;
  origin.px = 0.0f;
  origin.py = 0.0f;
  origin.pz = 0.0f;
  origin.pSize = 10.0f;
  datObj->addPoint(origin);
  datObj->Modified();
}
