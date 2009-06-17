#ifndef READ_ONLY_MODE_H
#define READ_ONLY_MODE_H

#include "genericMode.h"

class ReadOnlyMode : public GenericMode {
 public:
  ReadOnlyMode();
  virtual ~ReadOnlyMode();

  // Init
  bool init(GeometrySender* geom, struct arguments* args);

  // Start the mode running.
  void runMode();
 private:
  int m_numPlanes;
};

#endif
