#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtLabelRenderObject.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    
    rtRenderObject* label;
    rtRenderObject* label2;

    rtTimeManager::instance().startRenderTimer(&mainWin, 40);
    rtObjectManager::instance().setMainWinHandle(&mainWin);

    label = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel");
    label2 = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel2");

    mainWin.show();
    return app.exec();
}
