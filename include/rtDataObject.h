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
#ifndef RT_DATA_OBJECT_H
#define RT_DATA_OBJECT_H

#include <QString>
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <vtkProperty.h>

#include "objTypes.h"

//! Data object base class [abstract class]
/*!
  All data type objects in Vurtigo are derived from this class. All data in Vurtigo is arranged into a set of objects. For each object there is a data part and a render part. The data part maintains the data elements that define the object. The render part creates and manages the pipeline that displays the object either in 2D or 3D. This is the base class to all of the data parts.
  */
class rtDataObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtDataObject();

  //! Get the id for this object
  int getId();

  //! Set the id for this object
  void setId(int id);
  
  //! Get the displayed name for this object
  QString getObjName();
  //! Set the displayed name for this object
  void setObjName(QString name);

  //! Get the type of data object
  QString getObjectType();

  //! Set the type of data object
  void setObjectType(QString type);

  //! Check if the object is read only
  bool isReadOnly() { return m_readOnly; }

  //! Set the object to be read only
  void setReadOnly() { m_readOnly = true; }

  //! Set the object to be both readable and writeable
  void setReadWrite() { m_readOnly = false; }

  //! Set the read only flag
  /*!
    \param f The read only flag. If true the object becomes read only.
    */
  void setReadFlag(bool f) { m_readOnly = f; }

  //! Get access to the pointer to the base widget.
  /*!
    Every object type in Vurtigo can have a GUI that goes with it. The GUI is used to adjust settings specific to each type of object. To connect this object specific GUI with the general Vurtigo interface each object has access to a base widget. All object specific GUI is placed on that base widget. GUI elements not placed on this base widget cannot be shown in Vurtigo. The base widget is automatically inserted into the Vurtigo interface when the user clicks on a specific object.
    \return The pointer to the base widget.
    */
  virtual QWidget* getBaseWidget() { return &m_baseWidget; }

  //! Get the last time the object was modified.
  QDateTime getModified();

  //! Update the GUI with the modified data object info.
  virtual void update() = 0;

  //! Lock the object.
  /*!
    A locked object cannot be rendered. Objects must be locked before they are modified so as to avoid making modifications during a rendering. Once the modifications are complete the object must be unlocked. Note that this is a BLOCKING call. If the lock is not available the caller will wait until it becomes available.
    \sa unlock()
    */
  void lock() { m_objectLock.lock(); }

  //! Try to lock the object.
  /*!
    A locked object cannot be rendered. Objects must be locked before they are modified so as to avoid making modifications during a rendering. Once the modifications are complete the object must be unlocked. Unlike the lock function this is not a blocking call. This function will return immediately. If the return value is true then the lock has been acquired and the caller can modify the object and then call unlock. If the return is false then the lock was not acquired and so the object should not be modified. If the lock is not acquired then unlock should not be called.
    \return True if the lock was acquired and false otherwise. Call unlock only if true.
    \sa lock()
    \sa unlock()
    */
  bool tryLock() { return m_objectLock.tryLock(); }

  //! Try to lock the object and timeout if unable to do so.
  /*!
    Similar to the tryLock() function except that this function will not return immediately upon failure. This function will continue to try to get the lock for the specified number of miliseconds. If the lock is unobtainable in the alloted time then the function returns false.
    \param timeout The number of miliseconds before the function times out.
    \return True if the lock was acquired and false otherwise.
    \sa tryLock()
    */
  bool tryLock(int timeout) { return m_objectLock.tryLock(timeout); }

  //! Unlock the object
  /*!
    Release the hold on this object to allow it to be modified by other threads and to allow it to be rendered. Should only be called after a lock() or a successful tryLock() call. Calling the unlock in a different thread than the one that called the lock command results in an error.
    \sa lock()
    */
  void unlock() { m_objectLock.unlock(); }

  //! Save this object to file.
  /*!
    Save the data part of this object to a file. Function is implemented for each object type.
    \param file The file to save to.
    \return True if the file was saved as requested and false otherwise.
    */
  virtual bool saveFile(QFile* file) { return false; }

  //! Load this object from a file.
  /*!
    Load the data part f this object from a file. Function is implemented for each object type.
    \param file The file to load from.
    \return True if the file was loaded properly and false otherwise.
    */
  virtual bool loadFile(QFile* file) { return false; }

  //! Save the data file header for this object
  /*!
    All saved object files have a starting header that specifies the object type and the object name.
    \param writer The XML stream where the header will be written to.
    */
  void saveHeader(QXmlStreamWriter *writer);

  //! Save a header to an XML stream
  /*!
    Save a header with an object type and an object name to a given stream.
    \param writer The XML stream to save to
    \param type The object type to save
    \param name The object name to save
    */
  static void saveHeader(QXmlStreamWriter *writer, QString type, QString name);

  //! Load a header from a file to get the object type and name
  /*!
    If the read fails the object type will be rtConstants::OT_None.
    \param reader The XML stream to read from
    \param type The variable where the object type will be stored. OT_None if the read failed.
    \param name The variable where the object name will be stored. Blank string if the name read has failed.
    */
  static void loadHeader(QXmlStreamReader *reader, QString &type, QString &name);


  //! Save a vtkProperty type of object.
  static void saveVtkProperty(QXmlStreamWriter *writer, vtkProperty* prop, QString name);

  //! Load a vtkProperty type of object.
  static void loadVtkProperty(QXmlStreamReader *reader, vtkProperty* prop, QString &name);

  //! Convert a string to an integer or use a defualt value.
  static int getIntFromString(QString str, int defVal=0);

  //! Convert a string to a double or use a default value.
  static double getDoubleFromString(QString str, double defVal=0.0);

 public slots:
  //! The object was just modified. Adjust the time.
  void Modified();
  //! Update the GUI with the current internal data.
  void updateGUI();

 signals:
  //! Signal is sent when an object has been modified.
  void objectChanged(int);

 protected:
  //! Protected constructor
  rtDataObject();

  //! Setup the GUI
  virtual void setupGUI() = 0;
  //! Cleanup the GUI
  virtual void cleanupGUI() = 0;

  //! Mutex that locks the object.
  /*!
    Only one thread may modify (or render) this object at a time. The main purpose of this lock is to avoid modifying an object that is in the process of being rendered.
    */
  QMutex  m_objectLock;

 private:
  //! The object ID
  /*!
    The object id is provided by the object manager and is unique for this object. Object ids for consecutive object need not be sequential.
    */
  int m_objId;

  //! Object Name
  /*!
    The object name is not unique and should not be used to identify the object. The name is displayed for the user to see when they choose an object from a list.
    */
  QString m_objName;

  //! Object type
  QString m_objType;

  //! Flag is true if the object is read only.
  bool m_readOnly;

  //! The base widget where all of the GUI elements for each object will be placed.
  QWidget m_baseWidget;

  //! Time when this object was last modified.
  QDateTime m_modifyTime;
};

#endif
