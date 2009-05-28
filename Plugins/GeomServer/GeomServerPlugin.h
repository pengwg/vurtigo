#ifndef GEOM_SERVER_PLUGIN_H
#define GEOM_SERVER_PLUGIN_H

#include "rtPluginInterface.h"
#include "genericMode.h"
#include "sender.h"

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
  GeometrySender * sender;
  struct arguments args;
  GenericMode * mode;

  //! Sets default arguments for args
  void setDefaults();
  //! Connect and print appropriate message to console
  bool connectAndMessage(GeometrySender * sender);
  //! Read the information
  void retrieveInfo();
};

#endif
