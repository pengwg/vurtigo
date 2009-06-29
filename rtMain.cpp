#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
#include "rtBaseHandle.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    
    rtTimeManager::instance().startRenderTimer(&mainWin, 40);
    rtObjectManager::instance().setMainWinHandle(&mainWin);
    rtBaseHandle::instance(); // Important to create the object in THIS THREAD.

    int widID[2];
    widID[0] = mainWin.createNew2DWidget();
    widID[1] = mainWin.createNew2DWidget();

    mainWin.show();
    return app.exec();
}
