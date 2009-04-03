#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtLabelRenderObject.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtObjectManager objManager;
    rtMainWindow mainWin;

    rtRenderObject* label;
    rtRenderObject* label2;

    objManager.setMainWinHandle(&mainWin);
    mainWin.setObjectManager(&objManager);
    label = objManager.addObjectOfType(rtConstants::OT_TextLabel, "MyLabel");
    label2 = objManager.addObjectOfType(rtConstants::OT_TextLabel, "MyLabel2");

    mainWin.show();
    return app.exec();
}
