#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtLabelRenderObject.h"
#include "rtTimeManager.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtObjectManager objManager;
    rtMainWindow mainWin;
    rtTimeManager timeMan;

    rtRenderObject* label;
    rtRenderObject* label2;

    timeMan.startRenderTimer(&mainWin, 40);
    objManager.setMainWinHandle(&mainWin);
    mainWin.setObjectManager(&objManager);
    label = objManager.addObjectOfType(rtConstants::OT_TextLabel, "MyLabel");
    label2 = objManager.addObjectOfType(rtConstants::OT_TextLabel, "MyLabel2");


    mainWin.show();
    return app.exec();
}
