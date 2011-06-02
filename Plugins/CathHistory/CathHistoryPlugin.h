#ifndef CATH_HISTORY_PLUGIN_H
#define CATH_HISTORY_PLUGIN_H

#include "rtPluginInterface.h"

class CathHistoryPlugin : public QObject, public DataInterface {

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
