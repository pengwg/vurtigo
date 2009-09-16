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
