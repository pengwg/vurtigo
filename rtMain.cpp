#include <QApplication>
 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtObjectManager objManager;
    rtMainWindow mainWin;

    objManager.setMainWinHandle(&mainWin);

    mainWin.show();
    return app.exec();
}
