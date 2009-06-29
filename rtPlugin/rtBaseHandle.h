#ifndef RT_BASE_HANDLE_H
#define RT_BASE_HANDLE_H

#include "objTypes.h"
#include <QList>
#include <QString>
#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QThread>

class rtDataObject;

//! The handle that the Plugin can use. [Singleton]
/*!
  Plugins may need to ask the base for instances of object or they may need to register for callbacks. This base handle is the way plugins should access the base. The functions here are specifically designed for the plugins to use.
  It is higly reccomnded that the plugins SHOULD NOT use the other singleton classes (ex. rtObjectManager or rtPluginLoader) directly. There are plugin-specific safety features set in the rtBaseHandle class that protect the functioning of the base. If the other classes are used directly then these features are bypassed and as a result the plugin can cause bugs in the base. 
  This class is also a singleton meaning that there will only be one instance of it for the whole program. Plugins should get access to this via the instance() function.
 */
class rtBaseHandle : public QObject {

 Q_OBJECT

 public:
  //! Destructor
  ~rtBaseHandle();

  //! Get the instance of rtBaseHandle
  static rtBaseHandle& instance() {
    static rtBaseHandle handle;
    return handle;
  }

  int requestNewObject(rtConstants::rtObjectType objType, QString name="pluginObj");
  bool removeObject(int ID);
  QList<int> getObjectsOfType(rtConstants::rtObjectType objType);
  int getNumObjectsOfType(rtConstants::rtObjectType objType);
  rtDataObject* const getObjectWithID(int ID);
  const rtDataObject* const getROObjectWithID(int ID);
  bool watchClick(int pluginID, bool watch);

  void forceRenderUpdate(int objID);

 public slots:
  void requestNewObjectSlot(rtConstants::rtObjectType, QString);
 signals:
  void requestNewObjectSignal(rtConstants::rtObjectType, QString);

 protected:
  void connectSignals();

  int m_newObjectID;
  QMutex m_newObjectLock;
  QSemaphore m_newObjectWait;

  QThread* m_masterThreadPointer;

 private:
  //! Private constructor
  rtBaseHandle();
  //! Private copy-constructor
  rtBaseHandle(const rtBaseHandle&);
  rtBaseHandle& operator=(const rtBaseHandle&);

};

#endif
