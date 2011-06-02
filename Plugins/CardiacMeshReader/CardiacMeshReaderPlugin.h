#ifndef CARDIAC_MESH_READER_PLUGIN_H
#define CARDIAC_MESH_READER_PLUGIN_H

#include "rtPluginInterface.h"

class CardiacMeshReaderPlugin : public QObject, public DataInterface {

  Q_OBJECT
  Q_INTERFACES(DataInterface)

 public:
  void objectModified(int objID);
  bool init();
  void cleanup();
  void update();
  void point3DSelected(double px, double py, double pz, int intensity);

 private:
};

#endif
