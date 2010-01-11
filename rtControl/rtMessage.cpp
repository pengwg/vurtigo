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


rtMessage& rtMessage::instance() {
  static rtMessage handle;
  return handle;
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
