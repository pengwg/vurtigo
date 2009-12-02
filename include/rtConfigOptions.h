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

//! Container class for all of the configuration options that are read from a file.
/*!
The purpose of this class is to load a config file and store the information contained inside.
This is also a singleton class and so there will be only one instance of it throughout Vurtigo.
The reason for this is to ensure that the whole program uses the same config options.
It is recommended that the config options not be modified directly by classes within Vurtigo as all parameter changes will effect the rest of the program in unexpected ways.
  */
class rtConfigOptions
{
 public:
  //! Config Options Destructor
  ~rtConfigOptions();

  //! Get the refernce to this object
  static rtConfigOptions& instance() {
    static rtConfigOptions handle;
    return handle;
  }

  //! Load a config file
  /*!
    Load all of the parameters from a new config file. Note that any previous settings will be overwritten and lost.
    This function should be called at least once as part of the initialization of this class. Default parameter values are not useful and may cause problems.
    \param fName The full path name of the config file to load.
    \return true if the file was loaded and false if there was an error. Note that a return value of false may leave the internal data in an inconsistent state.
    \sa getConfigFileName()
    */
  bool loadFile(QString fName);

  //! Get the name of the currently loaded file.
  /*!
    \return The name of the config file that is currently loaded. A blank string is returned if no file was loaded or if the last load attempt failed.
    \sa loadFile()
   */
  QString getConfigFileName();

  QString getWLFileName();
  QString getHelpCollectionFileName();

 protected:
  //! The name of the file that is loaded.
  /*!
    The name of the config file that is currently loaded. This is a blank string if no file was loaded or if the last load attempt failed.
    \sa loadFile()
    \sa getConfigFileName()
    */
  QString m_fileName;

  //! The window and level config file.
  /*!
    The full path and file name of the config file for the window and level data. Empty string by default.
    */
  QString m_wlFileName;

  //! The name of the help collection file. Empty string by default.
  QString m_helpCollectionFile;

 private:
  //! Private Constructor
  /*!
  Only one instance of this class is permitted. Use instance() to get a reference to this object.
  \sa instance()
  */
  rtConfigOptions();
  //! Private Copy Constructor
  /*!
  Only one instance of this class is permitted. Use instance() to get a reference to this object.
  \sa instance()
  */
  rtConfigOptions(const rtConfigOptions&);
  //! Private Operator Equals
  /*!
  Only one instance of this class is permitted. Use instance() to get a reference to this object.
  \sa instance()
  */
  rtConfigOptions& operator=(const rtConfigOptions&);
};

#endif // RTCONFIGOPTIONS_H
