/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
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

#include "vtkRenderWindow.h"
#include "vtkImageData.h"

//! The handle that the Plugin can use. [Singleton]
/*!
  Plugins may need to ask the base for instances of object or they may need to register for callbacks. This base handle is the way plugins should access the base. The functions here are specifically designed for the plugins to use.
  It is higly reccomnded that the plugins SHOULD NOT use the other singleton classes (ex. rtObjectManager or rtPluginLoader) directly. There are plugin-specific safety features set in the rtBaseHandle class that protect the functioning of the base. If the other classes are used directly then these features are bypassed and as a result the plugin can cause bugs in the base. 
  This class is also a singleton meaning that there will only be one instance of it for the whole program. Plugins should get access to this via the instance() function.
  Functions in this class are usually thread safe. Those functions that are not thread safe are specified in the docs for that function.
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
  vtkImageData* grabScreenshot();
  vtkRenderWindow* getRenderWindow();

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

  vtkImageData* m_screen;

 private:
  //! Private constructor
  rtBaseHandle();
  //! Private copy-constructor
  rtBaseHandle(const rtBaseHandle&);
  rtBaseHandle& operator=(const rtBaseHandle&);

};

#endif
