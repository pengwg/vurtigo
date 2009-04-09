#ifndef RT_DATA_OBJECT_H
#define RT_DATA_OBJECT_H

#include <QString>
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QDateTime>
#include "objTypes.h"

class rtDataObject : public QObject {

Q_OBJECT

 public:
  ~rtDataObject();

  int getId();
  void setId(int id);
  
  QString getObjName();
  void setObjName(QString name);

  rtConstants::rtObjectType getObjectType();
  void setObjectType(rtConstants::rtObjectType ot);

  bool isReadOnly() { return m_readOnly; }
  void setReadOnly() { m_readOnly = true; }
  void setReadWrite() { m_readOnly = false; }
  void setReadFlag(bool f) { m_readOnly = f; }

  virtual QWidget* getBaseWidget() { return &m_baseWidget; }
  virtual QPushButton* getApplyButton() { return m_applyButton; }

  void Modified();
  QDateTime getModified();

  //! Update the GUI with the modified data object info.
  virtual void update() = 0;

 public slots:
  //! The apply button has been pressed. 
  virtual void apply() = 0;

 protected:
  rtDataObject();

 private:
  //! The object ID
  int m_objId;
  //! Object Name
  QString m_objName;
  rtConstants::rtObjectType m_objType;
  bool m_readOnly;
  QWidget m_baseWidget;
  QPushButton* m_applyButton;

  //! Time when this object was last modified.
  QDateTime m_modifyTime;
};

#endif
