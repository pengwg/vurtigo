#ifndef GEOM_SERVER_PLUGIN_H
#define GEOM_SERVER_PLUGIN_H

#include "rtPluginInterface.h"
#include "Converter.h"
#include "SenderSimp.h"

#define UPDATE_TIME 5000

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
   Converter * converter;
   SenderSimp * sender;

    //! Read the information
    void retrieveInfo();
};

#endif
