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

#include <iostream> 
#include <string>
#include <sstream>

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
#include "rtBaseHandle.h"
#include "rtMessage.h"

#include "version.h"

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
    rtMessage::instance().setStream(&(std::cout));

    std::cout << "Vurtigo Copyright (C) 2009 Sunnybrook Health Sciences Centre. ";
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY; ";
    std::cout << "This is free software, and you are welcome to redistribute it under certain conditions; ";
    std::cout << "See COPYING and COPYING.LESSER for details.\n\n" << std::endl;

    // Start at 1 since the first element is the name of the program
    for (int ix1=1; ix1<args.count(); ix1++) {
      if (args.at(ix1) == "--version" || args.at(ix1) == "-v") {
        std::stringstream msg;
        msg << "Vurtigo Version: " << VURTIGO_MAJOR_VERSION << "." << VURTIGO_MINOR_VERSION;
        rtMessage::instance().log(msg.str());
        runVurtigo = false; // Just checking the version no need to run the software.
      }
    }

    if (runVurtigo) {
      rtTimeManager::instance().startRenderTimer(&mainWin, 40);
      rtObjectManager::instance().setMainWinHandle(&mainWin);
      rtBaseHandle::instance(); // Important to create the object in THIS THREAD.

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
