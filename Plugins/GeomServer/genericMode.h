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
#ifndef GENERIC_MODE_H
#define GENERIC_MODE_H

#include "geom_client.h"
#include "sender.h"
#include "arguments.h"

#include <QList>
#include <QSemaphore>

#include "SenderThread.h"

class Converter;

//** Defines the superclass for all modes. This class cannot and should not be instantiated directly.
class GenericMode{
 private:

 protected:
  // Variables that are common to all modes.
  //! Object that communicates with the Geometry Server.
  GeometrySender* m_sender;
  //! All data related to the image.
  std::vector<IMAGEDATA> m_imgDataArray;
  //! All data related to the catheter
  std::vector<CATHDATA> m_cathDataArray;
  int m_planeID;

  QList<SenderThread::ListObject> m_planeList;
  QList<SenderThread::ListObject> m_cathList;

  Converter* converter;
  QSemaphore m_planeListLock;
  QSemaphore m_cathListLock;

 public:
  // This class cannot be created directly.
  GenericMode();
  virtual ~GenericMode();

  // Init the mode based on the command line arguments.
  bool init(GeometrySender* geom, struct arguments* args);

  // Start the mode running.
  void runMode();

  // Print the data.
  void print();
  bool sendPlanePosition(int id);
  bool receivePlanePosition(int id);
  bool receivePlane(int id);
  bool receivePlaneAndPosition(int id);
  bool receiveCatheter();

  void setPlaneList(QList<SenderThread::ListObject>  plane);
  void setCathList(QList<SenderThread::ListObject> cath);

  std::vector<CATHDATA> & getCaths() {
    return m_cathDataArray;
  }

  std::vector<IMAGEDATA> & getImages() {
    return m_imgDataArray;
  }
};


#endif
