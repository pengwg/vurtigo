#ifndef RT_MESSAGE_CPP
#define RT_MESSAGE_CPP

#include <iostream>
#include <ostream>
#include <string>

#include <QTextStream>

//! The main message system for Vurtigo.
/*!
  This class supports five different types of messages: Error, Warning, Debug, Benchmarking, and Log. Error messages are on by default. There should in theory only be one instance of this class for Vurtigo. While this is NOT strictly enforced (ex. by using a singleton) it is considered bad practice to create two instances of this class. The rtApplication creates one instance of the class. All classes that wish to use the messaging functionality should obtain that instance from rtApplication.

The messaging system has been designed such that the output stream may be modified. It is the responsability of the creating class (very likely rtApplication) to call setStream(ptr) to set a stream and then setStream(NULL) just before deleting the stream. Since rtMessage DOES NOT take ownership of the pointer it is the responsability of the class that called setStream(ptr) to both create and delete the pointer 'ptr'.

Note that the boolean flags that determine which messages are to be displayed are currently public.
  \sa rtApplication
  */
class rtMessage {
 public:
  //! Constructor
  rtMessage();

  //! Destructor
  ~rtMessage();

  //! Set the text stream pointer for the output stream.
  /*!
    When giving the rtMessage class a pointer to use as a stream it is the responsability of the caller of the function to create and delete the pointer correctly. Also, just before the pointer is deleted, setStream(NULL) should be called.
    \param stream The new input stream. If this is NULL the messaging stream is turned off and not more messages will be displayed.
    */
  inline void setStream(QTextStream *stream) { m_out = stream; }

  //! Report an error
  /*!
    \param lineNum The line number where the error occured. Can use __LINE__ to define this in general.
    \param fileName The name of the file where the error occured. Can use __FILE__ to define this in general.
    \param text A short message about the error.
    */
  void error(int lineNum, std::string fileName, std::string text);

  //! Report an error
  /*!
    \param lineNum The line number where the error occured. Can use __LINE__ to define this in general.
    \param fileName The name of the file where the error occured. Can use __FILE__ to define this in general.
    \param text A short message about the error.
    */
  void error(int lineNum, QString fileName, QString text);

  //! Report a warning
  /*!
    \param lineNum The line number where the warning occured. Can use __LINE__ to define this in general.
    \param fileName The name of the file where the warning occured. Can use __FILE__ to define this in general.
    \param text A short message about the warning.
    */
  void warning(int lineNum, std::string fileName, std::string text);

  //! Report a warning
  /*!
    \param lineNum The line number where the warning occured. Can use __LINE__ to define this in general.
    \param fileName The name of the file where the warning occured. Can use __FILE__ to define this in general.
    \param text A short message about the warning.
    */
  void warning(int lineNum, QString fileName, QString text);

  //! Print a debug statement
  /*!
    \param text The statement text
    */
  void debug(std::string text);

  //! Print a debug statement
  /*!
    \param text The statement text
    */
  void debug(QString text);

  //! Print a benchmark
  /*!
    \param text The benchmark text
    */
  void bench(std::string text);

  //! Print a benchmark
  /*!
    \param text The benchmark text
    */
  void bench(QString text);

  //! Print a log message
  /*!
    \param text The message text.
    */
  void log(std::string text);
  //! Print a log message
  /*!
    \param text The message text.
    */
  void log(QString text);

  //! Display error messages when true
  bool m_err;

  //! Display warnings when true
  bool m_warn;

  //! Display debug messages when true
  /*!
    Debug messages are off by default. However, when compiling with DEBUG_VERBOSE_MODE_ON they become on by default.
    */
  bool m_debug;

  //! Display benchmarks when true
  bool m_bench;

  //! Dispaly log messages when true
  bool m_log;

 protected:
  //! A pointer to the output stream.
  /*!
    When this class is created this pointer is set to NULL. It is not possible to display messages through this system while the pointer is NULL.
    To give the pointer a value use the setStream() function. The rtMessage class DOES NOT take ownership of this pointer and when the pointer is deleted it is important to call setStream(NULL).
    \sa setStream(QTextStream*)
    */
  QTextStream* m_out;
 
 private:
  //! Private copy constructor. It is bad practice to make a copy of rtMessage.
  rtMessage(const rtMessage&);
  //! Private equals operator. It is bad practice to make a copy of rtMessage.
  rtMessage& operator=(const rtMessage&);
};
 

#endif
