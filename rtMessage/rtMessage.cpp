#include "rtMessage.h"
#include "buildParam.h"

#include <QString>

rtMessage::rtMessage() {

  // No output set yet...
  m_out = NULL;

  m_err = true;
  m_warn = false;

#ifdef DEBUG_VERBOSE_MODE_ON
  m_debug = true;
#else
  m_debug = false;
#endif

  m_bench = false;
  m_log = true;
}

rtMessage::~rtMessage() {
  m_err = false;
  m_warn = false;
  m_debug = false;
  m_bench = false;
  m_log = false;
}

void rtMessage::error(int lineNum, std::string fileName, std::string text) {
  if (!m_err || !m_out) return;
  (*m_out) << "ERROR: On line: " << lineNum << " in " << fileName.c_str() << "\n";
  (*m_out) << text.c_str() << "\n";
  m_out->flush();
}

void rtMessage::error(int lineNum, QString fileName, QString text) {
  if (!m_err || !m_out) return;
  (*m_out) << "ERROR: On line: " << lineNum << " in " << fileName << "\n";
  (*m_out) << text << "\n";
  m_out->flush();
}

void rtMessage::warning(int lineNum, std::string fileName, std::string text) {
  if(!m_warn || !m_out) return;

  (*m_out) << "Warning: On line: " << lineNum << " in " << fileName.c_str() << "\n";
  (*m_out) << text.c_str() << "\n";
  m_out->flush();
}

void rtMessage::warning(int lineNum, QString fileName, QString text) {
  if(!m_warn || !m_out) return;

  (*m_out) << "Warning: On line: " << lineNum << " in " << fileName << "\n";
  (*m_out) << text << "\n";
  m_out->flush();
}

void rtMessage::debug(std::string text) {
  if (!m_debug || !m_out) return;
  (*m_out) << "Debug: " << text.c_str() << "\n";
  m_out->flush();
}

void rtMessage::debug(QString text) {
  if (!m_debug || !m_out) return;
  (*m_out) << "Debug: " << text << "\n";
  m_out->flush();
}

void rtMessage::bench(std::string text) {
  if(!m_bench || !m_out) return;
  (*m_out) << "Benchmark: " << text.c_str() << "\n";
  m_out->flush();
}

void rtMessage::bench(QString text) {
  if(!m_bench || !m_out) return;
  (*m_out) << "Benchmark: " << text << "\n";
  m_out->flush();
}

void rtMessage::log(std::string text) {
  if(!m_log || !m_out) return;
  (*m_out) << "Log: " << text.c_str() << "\n";
  m_out->flush();
}

void rtMessage::log(QString text) {
  if(!m_log || !m_out) return;
  (*m_out) << "Log: " << text << "\n";
  m_out->flush();
}
