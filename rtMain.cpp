#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

#include <iostream> 
#include <sstream>
#include <cstdio>

#include "rtApplication.h"

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"
#include "rtBaseHandle.h"
#include "rtMessage.h"
#include "rtConfigOptions.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int exitCode = 0;

    rtApplication::instance().displayCopyright();
    rtApplication::instance().initApp( app.arguments() );    

    if ( rtApplication::instance().runVurtigo() ) {
      exitCode = app.exec();
    }
    return exitCode;
}


