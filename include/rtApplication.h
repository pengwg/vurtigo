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
#ifndef RTAPPLICATION_H
#define RTAPPLICATION_H

#include <QTextStream>
#include <QObject>

class rtMainWindow;
class rtPluginLoader;

//! The main application class [singleton]
/*!
  */
class rtApplication : public QObject
{
  Q_OBJECT
public:
  ~rtApplication();

  static rtApplication& instance();

  void displayCopyright();
  void initApp();

  rtPluginLoader* const getPluginLoader() { return m_pluginLoader; }
  rtMainWindow* const getMainWinHandle() { return m_mainWindow; }

public slots:
  void cleanupApp();

protected:
  QTextStream* m_stream;
  rtPluginLoader* m_pluginLoader;
  rtMainWindow* m_mainWindow;

private:
  rtApplication();
  rtApplication(const rtApplication&);
  rtApplication& operator=(const rtApplication&);
};

#endif // RTAPPLICATION_H
