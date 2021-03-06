/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef RT_PLUGIN_INTERFACE_H
#define RT_PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QWidget>
#include <QDateTime>

class DataInterface {
 public:
  ~DataInterface() {}

  //! Get the base widget that controls this plugin.
  QWidget* getWidget() { return m_mainWidget; }

  //! Assign a unique ID to this plugin.
  void setUniqueID(int  ID) { m_uniqueID = ID; }

  //! Query the unique ID for this plugin.
  int getUniqueID() { return m_uniqueID; }

  //! Add an object that this plugin will watch.
  void addObjectToWatchList(int ID) {
    if (!m_objectList.contains(ID)) {
      m_objectList.append(ID);
    }
  }

  //! Remove an object from the list of things to watch for this plugin.
  void removeObjectFromWatchList(int ID) {
    if (m_objectList.contains(ID)) {
      m_objectList.removeAll(ID);
    }
  }

  //! Check if an object is in the watch list.
  bool isInWatchList(int ID) {
    return m_objectList.contains(ID);
  }

  //! Set the update time for this plugin.
  /*!
    The update time is defined by the number of miliseconds between calls to update. The base will try to update at the desired rate but that is not guaranteed.
    The minimum update time is 15 milisecs. Lower times will be defaulted to 15 milisecs. If the set time is negative the update function will never be called.
    The default value is -1 which means that update is not called by default.
    @see getUpdateTime()
    \param time The update time.
    */
  void setUpdateTime(int time) {
    if (time<0) {
      m_updateTime = -1;
    } else if (time<15) {
      m_updateTime = 15;
    } else {
      m_updateTime = time;
    }
  }

  //! Get the update time for this plugin.
  /*!
    Get the number of milliseconds between consecutive calls to update.
    A value of -1 inidicates that the update function is not scheduled to be called.
    @see setUpdateTime()
    @return The Update time.
    */
  int getUpdateTime() {
    return m_updateTime;
  }

  //! Called when an object is modified in the base.
  /*!
    Plugin has to register with the base in order to get this call.
   */
  virtual void objectModified(int objID) = 0;

  //! This is the first thing that is called when a plugin is loaded.
  /*!
    Called by the base when the plugin is first loaded. This is a good place to allocate memory or to start threads.
    @return Flase if the plugin could not be initialized and true otherwise.
   */
  virtual bool init() = 0;

  //! Called just before the plugin is unloaded by the base.
  /*!
    This is where memory previously allocated should be cleaned and where extra running threads can be terminated.
   */
  virtual void cleanup() = 0;

  //! Check to see if this plugin needs to be updated.
  /*!
    This function generally gets called by the plugin loader. If the right time has elapsed then this function calls update directly.
    @see update()
    */
  void tryUpdate() {
    if (getUpdateTime() >= 0) {
      QDateTime time( m_lastUpdate );
      time = time.addMSecs(getUpdateTime());
      if (QDateTime::currentDateTime() > time) {
        update();
        m_lastUpdate = QDateTime::currentDateTime();
      }
    }
  }

  //! Base calls this function at regular intervals.
  /*!
    The plugin can register so that the base will call this function at regular intervals.
   */
  virtual void update() = 0;

  //! Called when the user selects a point in the 3D view.
  virtual void point3DSelected(double px, double py, double pz, int intensity) = 0;

 protected:
  DataInterface() { m_updateTime=-1; m_lastUpdate=QDateTime::currentDateTime(); m_mainWidget = NULL; }

  QWidget* m_mainWidget;
  int m_uniqueID;

 private:
  //! How many miliseconds must pass before a new call to update.
  int m_updateTime;

  //! Time when this plugin was last updated.
  QDateTime m_lastUpdate;

  //! Last time the objects were updated.
  QDateTime m_objectUpdate;
  //! List of objects to update.
  QList<int> m_objectList;
};

Q_DECLARE_INTERFACE(DataInterface, "rt.Vurtigo.DataInterface/1.0")

#endif
