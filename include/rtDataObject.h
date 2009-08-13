#ifndef RT_DATA_OBJECT_H
#define RT_DATA_OBJECT_H

#include <QString>
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QDateTime>
#include <QMutex>
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

  QDateTime getModified();

  //! Update the GUI with the modified data object info.
  virtual void update() = 0;

  void lock() { m_objectLock.lock(); }
  bool tryLock() { return m_objectLock.tryLock(); }
  bool tryLock(int timeout) { return m_objectLock.tryLock(timeout); }
  void unlock() { m_objectLock.unlock(); }

 public slots:
  void Modified();
  void updateGUI();

 signals:
  //! Signal is sent when an object has been modified.
  void objectChanged();

 protected:
  rtDataObject();

  //! Setup the GUI
  virtual void setupGUI() = 0;
  //! Cleanup the GUI
  virtual void cleanupGUI() = 0;

  QMutex  m_objectLock;

 private:
  //! The object ID
  int m_objId;
  //! Object Name
  QString m_objName;
  rtConstants::rtObjectType m_objType;
  bool m_readOnly;
  QWidget m_baseWidget;

  //! Time when this object was last modified.
  QDateTime m_modifyTime;
};

#endif
