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
#include "SenderSimp.h"
#include <iostream>

using namespace std;

//! Sets default arguments for args
void SenderSimp::setSenderDefaults() {
  char const * const hostname = "localhost";
  args.hostname = NULL;
  SenderSimp::copyString(&args.hostname, hostname);
  args.port = 1777;
}

SenderSimp::SenderSimp() {
  readMode = new GenericMode();
  tempConnect = false;

  setSenderDefaults();
  readMode->init(&sender, &args);
  // Put one resource into the semaphore.
  runLock.release();
}

SenderSimp::~SenderSimp() {
  delete [] args.hostname;
  delete (GenericMode *) readMode;
}

//! Connect and print appropriate message to console
bool SenderSimp::connectAndMessage() {
  if (isConnected()) {
    cout << "Already connected." << endl;
    return true;
  }
  else if (!sender.connect(args.hostname,args.port)) {
    cerr << "Error connecting to Geometry Server. Check that it is running on port " << args.port << " on host: " << args.hostname << endl;
    sender.disconnect();
    return false;
  } else {
    cout << "Connection with: " << args.hostname << ":" << args.port << " established." << endl;
    return true;
  }
}


//! Returns true if the sender is connected permanently (not temporarily)
bool SenderSimp::isConnected() {
  return sender.isConnected() && !tempConnect;
}

//! Disconnects from the server
void SenderSimp::disconnect() {
  if (sender.isConnected()) {
    sender.disconnect();
    cout << "Disconnected." << endl;
  }
  else {
    cout << "Not connected." << endl;
  }
}

//! Reads the information
void SenderSimp::runReadMode() {
  // Check that the connection has been made.
  if (!isConnected()) return;

  if (runLock.tryAcquire()) {
    readMode->runMode();
    runLock.release();
  }
}

//! Prints the information read
void SenderSimp::print() {
    readMode->print();
}

//! Returns the config information
arguments * SenderSimp::getArgs() {
  return &args;
}

//! Returns a vector of remote cathater objects
vector<CATHDATA> & SenderSimp::getCaths() {
  return readMode->getCaths();
}

//! Returns a vector of remote image objects
vector<IMAGEDATA> & SenderSimp::getImages() {
  return readMode->getImages();
}

//! Copys a string from src to dest
void SenderSimp::copyString(char ** const dest, const char * const src) {
  string srcCopy(src);
  if (*dest != NULL)
    delete [] *dest;
  *dest = new char[srcCopy.size()];
  strcpy(*dest, src);
}
