#ifndef RT_LABEL_RENDER_OPTIONS_H
#define RT_LABEL_RENDER_OPTIONS_H

#include "rtRenderOptions.h"
#include <QVBoxLayout>
#include <QLabel>

class rtLabelRenderOptions : public rtRenderOptions {
 public:
  rtLabelRenderOptions();
  ~rtLabelRenderOptions();

 protected:
  void setupGUI();
  void cleanupGUI();

 private:
  QVBoxLayout* m_vbox;
  QLabel* m_nameLabel;
};

#endif
