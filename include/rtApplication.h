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
  This is the main application class for vurtigo. There is only one instance of this class; strictly enforced by singleton. The rtApplication class serves as the central control node for Vurtigo. It stores the handles to each of the main components that make up Vurtigo:
  - The plugin loader rtPluginLoader
  - The main window rtMainWindow
  - The initial configuration options rtConfigOptions
  - The time/synch manager rtTimeManager
  - The messaging system rtMessage
  - The internal object manager rtObjectManager

Different components of Vurtigo will need to have access to the others. The best way to do this is by calling (for exmaple) rtApplication::instance().getPluginLoader() or rtApplication::instance().getMainWinHandle()...

Aside from maintaining the handles this class is also responsible for application startup and shutdown.
  */
class rtApplication : public QObject
{
  Q_OBJECT
public:
  //! Destructor
  ~rtApplication();

  //! Obtain an instance of this singleton class.
  static rtApplication& instance();

  //! Display the copyright message
  /*!
    Send the copyright message to the main message output stream.
    Note: This message DOES NOT pass through the rtMessage system.
      \sa m_stream
    */
  void displayCopyright();

  //! Initialize the application
  /*!
    This function is called on startup to do all the setup needed for Vurtigo to run.
    \param args The command line arguments as they are produced by the QApplication object. (See: QCoreApplication::arguments())
    */
  void initApp(QStringList args);

  //! Returns true if Vurtigo will run.
  /*!
    This function serves a special purpose. In some cases Vurtigo will be run with command line arguments (such as --help) where the user is only requesting information or other setup details and it is not necesary for Vurtigo to start. In that case this function will return false. Usually this is true.
    \sa m_runVurtigo
    */
  bool runVurtigo() { return m_runVurtigo; }

  //! Get a pointer to the plugin loader
  rtPluginLoader* const getPluginLoader() { return m_pluginLoader; }

  //! Get a pointer to the main window
  rtMainWindow* const getMainWinHandle() { return m_mainWindow; }

  //! Get a pointer to the config options
  rtConfigOptions* const getConfigOptions() { return m_configOptions; }

  //! Get a pointer to the time manager
  rtTimeManager* const getTimeManager() { return m_timeManager; }

  //! Get a pointer to the message system
  rtMessage* const getMessageHandle() { return m_message; }

  //! Get a pointer to the object manager
  rtObjectManager* const getObjectManager() { return m_objManager; }

public slots:
  //! Perform cleanup to shutdown Vurtigo
  /*!
    This slot is connected to the aboutToQuit() signal from the QCoreApplication. As Qt is about to quit the main event loop this function is called and Vurtigo does the cleanup that it requires.
    */
  void cleanupApp();

protected:
  //! The output stream
  /*!
    This stream pointer is passed to the rtMessage class as the standard Vurtigo output stream. As a result all messages from Vurtigo will pass through this stream.
    The default output stream is set to stdout.
  */
  QTextStream* m_stream;

  //! True if Vurtigo is set to run.
  /*!
    In some cases Vurtigo will be run with command line arguments (such as --help) where the user is only requesting information or other setup details and it is not necesary for Vurtigo to start. In those cases this variable is set to false.
    \sa runVurtigo()
    */
  bool m_runVurtigo;

  //! Pointer to the plugin loader.
  rtPluginLoader* m_pluginLoader;
  //! Pointer to the main window.
  rtMainWindow* m_mainWindow;
  //! Pointer to the config options.
  rtConfigOptions* m_configOptions;
  //! Pointer to the time manager.
  rtTimeManager* m_timeManager;
  //! Pointer to the message subsystem.
  rtMessage* m_message;
  //! Pointer to the object manager.
  rtObjectManager* m_objManager;

  //! Parse the command line arguments.
  void parseArgs(QStringList args);

  //! Create the 'origin' object for the 3D Vurtigo coordinate system.
  /*!
    Vurtigo uses a global 3D coordinate system. To aid users in their visualization an origin object is created.
    The only purpose of this object is to render to the (0,0,0) position to give a user the idea of where other objects are positioned in space.
    */
  void setupOrigin();

private:
  //! Private constructor. Singleton class.
  rtApplication();
  //! Private copy constructor. Singleton class.
  rtApplication(const rtApplication&);
  //! Private equals operator. Singleton class.
  rtApplication& operator=(const rtApplication&);
};

#endif // RTAPPLICATION_H
