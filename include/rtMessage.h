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
#ifndef RT_MESSAGE_CPP
#define RT_MESSAGE_CPP

#include <iostream>
#include <ostream>
#include <string>

#include <QTextStream>

class rtMessage {
 public:
  ~rtMessage();

  static rtMessage& instance();

  inline void setStream(QTextStream *stream) { m_out = stream; }

  void error(int lineNum, std::string fileName, std::string text);
  void error(int lineNum, QString fileName, QString text);

  void warning(int lineNum, std::string fileName, std::string text);
  void warning(int lineNum, QString fileName, QString text);

  void debug(std::string text);
  void debug(QString text);

  void bench(std::string text);
  void bench(QString text);

  void log(std::string text);
  void log(QString text);

  bool m_err; //<! Display error messages.
  bool m_warn;
  bool m_debug;
  bool m_bench;
  bool m_log;

 protected:
  QTextStream* m_out;
 
 private:
  rtMessage();
  rtMessage(const rtMessage&);
  rtMessage& operator=(const rtMessage&);
};
 

#endif
