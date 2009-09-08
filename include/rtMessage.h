#ifndef RT_MESSAGE_CPP
#define RT_MESSAGE_CPP

#include <iostream>
#include <ostream>
#include <string>

class rtMessage {
 public:
  ~rtMessage();

  static rtMessage& instance() {
    static rtMessage handle;
    return handle;
  }

  void setStream(std::ostream* stream) { m_out = stream; }

  void error(int lineNum, std::string fileName, std::string text);
  void warning(int lineNum, std::string fileName, std::string text);
  void debug(std::string text);
  void bench(std::string text);
  void log(std::string text);

  bool m_err; //<! Display error messages.
  bool m_warn;
  bool m_debug;
  bool m_bench;
  bool m_log;

 protected:
  std::ostream* m_out;
 
 private:
  rtMessage();
  rtMessage(const rtMessage&);
  rtMessage& operator=(const rtMessage&);
};
 

#endif
