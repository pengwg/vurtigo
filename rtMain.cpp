#include <QApplication>
#include <QStringList>

#include <iostream> 

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
#include "rtBaseHandle.h"

#include "version.h"

#include "rt3dPointBufferRenderObject.h"
#include "rt3dPointBufferDataObject.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    QStringList args = app.arguments();
    bool runVurtigo = true;
    int exitCode = 0;

    // Start at 1 since the first element is the name of the program
    for (int ix1=1; ix1<args.count(); ix1++) {
      if (args.at(ix1) == "--version" || args.at(ix1) == "-v") {
        std::cout << "Vurtigo Version: " << VURTIGO_MAJOR_VERSION << "." << VURTIGO_MINOR_VERSION << std::endl;
        runVurtigo = false; // Just checking the version no need to run the software.
      }
    }

    if (runVurtigo) {
      rtTimeManager::instance().startRenderTimer(&mainWin, 40);
      rtObjectManager::instance().setMainWinHandle(&mainWin);
      rtBaseHandle::instance(); // Important to create the object in THIS THREAD.

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
      renObj->tryUpdate();

      mainWin.show();
      exitCode = app.exec();
    }
    return exitCode;
}
