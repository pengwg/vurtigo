#ifndef GEOM_SERVER_PLUGIN_H
#define GEOM_SERVER_PLUGIN_H

#include "rtPluginInterface.h"
#include "Converter.h"
#include "SenderSimp.h"

//Time in milliseconds the the plug calls update()
#define UPDATE_TIME 100

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
   //! Maps the ids locally on and the server
   Converter * converter;
   //! Wrapper to simply sending information
   SenderSimp * sender;

    //! Read the information
    void retrieveInfo();
};

#endif
