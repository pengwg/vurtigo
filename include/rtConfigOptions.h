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
