#ifndef RT_RENDER_OPTIONS_H
#define RT_RENDER_OPTIONS_H

#include "objTypes.h"
#include <QWidget>

class rtRenderOptions {

 public:
  ~rtRenderOptions();

  rtConstants::rtObjectType getType() { return m_objType; }
  void setType(rtConstants::rtObjectType ot) { m_objType = ot; }

  QWidget* getBaseWidget() { return &m_baseWidget; }

 protected:
  rtRenderOptions();

 private:
  rtConstants::rtObjectType m_objType;
  QWidget m_baseWidget;
};

#endif
