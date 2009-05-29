#ifndef GEOM_SERVER_PLUGIN_H
#define GEOM_SERVER_PLUGIN_H

#include "rtPluginInterface.h"
#include "genericMode.h"
#include "sender.h"
#include "Converter.h"

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
  GeometrySender * sender;
  arguments args;
  GenericMode * mode;
  Converter * converter;

  //! Sets default arguments for args
  void setDefaults();
  //! Connect and print appropriate message to console
  bool connectAndMessage(GeometrySender * sender);

  //! Read the information
  void retrieveInfo();
};

#endif
