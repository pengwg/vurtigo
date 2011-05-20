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
#ifndef RTCONFIGOPTIONS_H
#define RTCONFIGOPTIONS_H

#include <QString>
#include <QSettings>
#include <QList>

//! Container class for all of the configuration options that are read from a file.
/*!
The purpose of this class is to load a config file and store the information contained inside.
It is recommended that the config options not be modified directly by classes within Vurtigo as all parameter changes will effect the rest of the program in unexpected ways.
  */
class rtConfigOptions
{
 public:
  //! Config options constructor.
  rtConfigOptions();
  //! Config Options Destructor
  ~rtConfigOptions();

  QString getWLFileName();
  QString getSourceCodeHelpFileName();
  QString getHelpCollectionFileName();
  QString getConfigPath() { return m_configPath; };
  QList<QString> getPluginList();

 protected:
  //! The path to the location of all of vurtigo's config details.
  QString m_configPath;

  //! The vurtigo settings object.
  QSettings *m_settings;


 private:
  //! Private Copy Constructor
  rtConfigOptions(const rtConfigOptions&);
  //! Private Operator Equals
  rtConfigOptions& operator=(const rtConfigOptions&);
};

#endif // RTCONFIGOPTIONS_H
