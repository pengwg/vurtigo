#include <QApplication>
 
#include "rtMainWindow.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
 
    mainWin.show();
    return app.exec();
}
