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
#include <QStringList>

class rtMainWindow;
class rtPluginLoader;
class rtConfigOptions;
class rtTimeManager;
class rtMessage;
class rtObjectManager;

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
  void initApp(QStringList args);

  //! Returns true if Vurtigo will run.
  bool runVurtigo() { return m_runVurtigo; }

  rtPluginLoader* const getPluginLoader() { return m_pluginLoader; }
  rtMainWindow* const getMainWinHandle() { return m_mainWindow; }
  rtConfigOptions* const getConfigOptions() { return m_configOptions; }
  rtTimeManager* const getTimeManager() { return m_timeManager; }
  rtMessage* const getMessageHandle() { return m_message; }
  rtObjectManager* const getObjectManager() { return m_objManager; }

public slots:
  void cleanupApp();

protected:
  QTextStream* m_stream;
  bool m_runVurtigo;

  rtPluginLoader* m_pluginLoader;
  rtMainWindow* m_mainWindow;
  rtConfigOptions* m_configOptions;
  rtTimeManager* m_timeManager;
  rtMessage* m_message;
  rtObjectManager* m_objManager;

  //! Parse the command line arguments.
  void parseArgs(QStringList args);
  void setupOrigin();

private:
  rtApplication();
  rtApplication(const rtApplication&);
  rtApplication& operator=(const rtApplication&);
};

#endif // RTAPPLICATION_H
