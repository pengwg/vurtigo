/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
