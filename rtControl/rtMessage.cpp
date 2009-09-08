#include "rtMessage.h"


rtMessage::rtMessage() {
  m_err = true;
  m_warn = false;
  m_debug = false;
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
  if (!m_err) return;
  (*m_out) << "ERROR: On line: " << lineNum << " in " << fileName << std::endl;
  (*m_out) << text << std::endl;
}

void rtMessage::warning(int lineNum, std::string fileName, std::string text) {
  if(!m_warn) return;

  (*m_out) << "Warning: On line: " << lineNum << " in " << fileName << std::endl;
  (*m_out) << text << std::endl;
}

void rtMessage::debug(std::string text) {
  if (!m_debug) return;
  (*m_out) << "Debug: " << text << std::endl;
}

void rtMessage::bench(std::string text) {
  if(!m_bench) return;
  (*m_out) << "Benchmark: " << text << std::endl;
}

void rtMessage::log(std::string text) {
  if(!m_log) return;
  (*m_out) << "Log: " << text << std::endl;
}
