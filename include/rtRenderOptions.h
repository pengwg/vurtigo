#ifndef RT_RENDER_OPTIONS_H
#define RT_RENDER_OPTIONS_H

#include "ui_rtRenderOptions.h"

#include <QHash>
#include <QString>

class rtRenderOptions : public QDialog, private Ui::rtRenderOptions {
 Q_OBJECT

public:
  rtRenderOptions();
  ~rtRenderOptions();

  void removeStereoFeature();

  int getRenQuality() { return m_renQuality; }
  int getRenUpdateQuality() { return m_renUpdateQuality; }
  int getStereoType() { return m_currStereo; }

  void setStereoType(int t);
  void setDirectRender(bool);
  void setGLRender(bool);

public slots:
  void renQuality(int);
  void renUpdateQuality(int);
  void stereoComboChanged(QString);

protected:
  int m_renQuality;
  int m_renUpdateQuality;

  int m_currStereo;
  QHash<QString, int> m_stereoTypes;

  bool m_directRender;
  bool m_glRender;
};

#endif
