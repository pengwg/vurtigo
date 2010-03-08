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
#ifndef GEOM_SERVER_PLUGIN_H
#define GEOM_SERVER_PLUGIN_H

#include <QMutex>

#include "rtPluginInterface.h"
#include "SenderThread.h"
#include "SenderSimp.h"
#include "Converter.h"

//Time in milliseconds the plugin calls update()
#define UPDATE_TIME 10

//! Plugin object for GeomServer
/*!
  Workflow from the plugin side and management of high level objects is done here.
 */
class GeomServerPlugin : public QObject, public DataInterface {

  Q_OBJECT
  Q_INTERFACES(DataInterface)

 public:
  void objectModified(int objID);
  bool init();
  void cleanup();
  void update();
  void point3DSelected(double px, double py, double pz, int intensity);
 private:
  //! Thread for GeomServerPlugin::readAndSetData()
  SenderThread senderThread;

  //! A lock to ensure that the update call is not flooded.
  QMutex m_singleUpdate;
};

#endif
