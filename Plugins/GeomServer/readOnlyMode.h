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
  bool m_prompt;
  bool m_outputImg;
  int m_numIter;
  int m_sleep;
  int m_numPlanes;
};

#endif
