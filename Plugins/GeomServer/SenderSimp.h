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
#ifndef SENDER_SIMP_H
#define SENDER_SIMP_H
#include "sender.h"
#include "genericMode.h"
#include <QString>
#include <QObject>
#include <QSemaphore>

//! A wrapping class for sending information
/*!
  Uses GeometrySender and GenericMode from geometryTester and simplifies it
  */
class SenderSimp : public QObject {
  Q_OBJECT

  public:
    SenderSimp();
    ~SenderSimp();
    bool isConnected();

    arguments * getArgs();
    std::vector<CATHDATA> & getCaths();
    std::vector<IMAGEDATA> & getImages();
    static void copyString(char ** const dest, const char * const src);

    GenericMode* getReaderHandle() { return readMode; }

    //! Check is a new signal can be sent.
    bool allocateNewSignal() { return m_signalsAvailable.tryAcquire(); }

   public slots:
    bool connectAndMessage();
    void disconnect();

    void print();
    void runReadMode();

protected:
  //! Sends info, needed by Generic Mode
  GeometrySender sender;
  //! Config information for connecting
  arguments args;
  //! For reading information
  GenericMode * readMode;
  //! true, if the application is connecting temporarily (for sending information)
  bool tempConnect;

  //! Semaphore to ensure only one read process is running at a time.
  QSemaphore runLock;

  void setSenderDefaults();

  QSemaphore m_signalsAvailable;
};

#endif
