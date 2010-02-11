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


