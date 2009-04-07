#ifndef RT_RENDER_OPTIONS_H
#define RT_RENDER_OPTIONS_H

#include "objTypes.h"
#include <QWidget>
#include <QPushButton>
#include <QObject>

class rtRenderOptions : public QObject{

Q_OBJECT

 public:
  ~rtRenderOptions();

  rtConstants::rtObjectType getType() { return m_objType; }
  void setType(rtConstants::rtObjectType ot) { m_objType = ot; }

  virtual QWidget* getBaseWidget() { return &m_baseWidget; }
  virtual QPushButton* getApplyButton() { return m_applyButton; }

 public slots:
  //! Takes data from the GUI
  virtual void apply() = 0;
  //! Takes data from the data object
  virtual void update() = 0;

 protected:
  rtRenderOptions();

 private:
  rtConstants::rtObjectType m_objType;
  QWidget m_baseWidget;
  QPushButton* m_applyButton;
};

#endif
