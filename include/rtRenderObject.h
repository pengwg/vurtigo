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
#ifndef RT_RENDER_OBJECT_H
#define RT_RENDER_OBJECT_H

#include <QString>
#include <QTreeWidgetItem>
#include <QObject>
#include <QDateTime>
#include <QList>
#include <QHash>
#include <QFile>

#include <vtkProp.h>
#include <vtkPropAssembly.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "rtApplication.h"
#include "rtMainWindow.h"

#include "objTypes.h"
class rtDataObject;
class rtMainWindow;

//! Render object base class [abstract class]
/*!
  All render type objects are derived from this class. Render objects construct VTK pipelines to visualize the object info in 2D or 3D.
  */
class rtRenderObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtRenderObject();

  //! Get the pointer to the data object
  /*!
    Each render object has a data object that it is paired with. This function will retrieve the pointer the the data object that corresponds to this render object.
    \return Pointer to the data object.
    */
  rtDataObject* getDataObject();

  //! Get the name of this render object.
  QString getName();

  //! Get the type of this render object
  /*!
    \return The type of this object
    \sa QString
    */
  QString getObjectType();

  QList<QString> get2DViewNameList() { return m_pipe2D.keys(); }
  bool viewWithNameExists(QString name) { return m_pipe2D.contains(name); }
  vtkProp* get2DViewWithName(QString name) { return m_pipe2D.value(name); }

  void setDataObject(rtDataObject* dataObj);

  void addSyncObject(rtRenderObject *rObj) {m_syncList.append(rObj);}
  void removeSyncObject(rtRenderObject *rObj);
  void removeAllSync() {m_syncList.clear();}

  //! Set the name for this data object
  void setName(QString renName);

  QTreeWidgetItem* getTreeItem() { return m_treeItem; }
  virtual void updateTreeItem();
  virtual void uncheckTreeItem();

  void setVisible3D(int window,bool v);
  void setVisible3DAll(QList<bool> v);
  void setVisible3DAllOn() { for(int ix1=0; ix1<m_visible3D.size(); ix1++) m_visible3D[ix1] = true;}
  void setVisible3DAllOff(){ for(int ix1=0; ix1<m_visible3D.size(); ix1++) m_visible3D[ix1] = false;}
  QList<bool> getVisible3D(){ return m_visible3D; }


  //! Save an object's 3D visibilities
  /*!
    All Objects have a list of visibilities for the current set of render windows
    \param writer The XML stream where the visibilities will be written to
    */
  void saveVisibilities(QXmlStreamWriter *writer);

  //! Load an object's 3D visibilities
  /*!
    All Objects have a list of visibilities for the current set of render windows
    \param reader The XML stream where the visibilities will be read from
    */
  void loadVisibilities(QXmlStreamReader *reader);

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren, int window) = 0;

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren, int window) = 0;

  //! Make this object a copy of another object
  virtual void copyObject(rtRenderObject *from) {}

  //! Create an instance of this Render Object (to be implemented by each derived class)
  virtual rtRenderObject *createObject() {return NULL;}

  bool tryUpdate();

  void resetUpdateTime();
  bool updateNeeded();

  //! Get the location of an object in the 3D space.
  virtual bool getObjectLocation(double loc[6]) = 0;

  //! Set the rendering quality.
  /*!
    Some objects are able to render at a lower quality based on speed requirements.
    \param quality The desired rendering quality. Values are in the range [0, 1] and will be clamped to this if they are outside these bounds.
    */
  virtual void setRenderQuality(double quality) { }

  //! Save an object to a file.
  virtual bool saveFile(QFile* file);

  //! Load this object from a file.
  virtual bool loadFile(QFile* file);

  //! deselect this object
  virtual void deselect() {}

 public slots:
  //! Called when one of the mouse buttons is pressed.
  virtual void mousePressEvent(QMouseEvent* event,int window) {
    if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
      rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMousePress(event,window);
  }
  //! Called when the mouse position changes and at least one button is pressed. (See QMouseEvent)
  virtual void mouseMoveEvent(QMouseEvent* event,int window) {
    if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
      rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseMove(event,window);
  }
  //! Called when a mouse button is released
  virtual void mouseReleaseEvent(QMouseEvent* event,int window) {
    if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
      rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseRelease(event,window);
  }
  //! Called when the mouse button is double clicked.
  virtual void mouseDoubleClickEvent(QMouseEvent* event,int window) {  }
  //! Called when a key is pressed.
  virtual void keyPressEvent(QKeyEvent* event,int window) { }
  //! Called when a key is released.
  virtual void keyReleaseEvent(QKeyEvent* event,int window) {}
  //! The mouse wheel was moved.
  virtual void wheelEvent(QWheelEvent* event,int window) {
    if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
      rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseWheel(event,window);
  }

 protected:
  //! The rtRenderObject constructor.
  /*!
    The protected rtRenderObject constructor. The rtRenderObject class is not meant to be instantiated. The proper use of this class is to derive from it a specific type of renderer. 
  */
  rtRenderObject();

  //! Set the type for this render object
  /*!
    \param objType The type of object to use
    \sa QString
    */
  void setObjectType(QString objType);

  //! Apply changes from the modified data object.
  virtual void update() = 0;

  //! Pointer to the data object
  rtDataObject* m_dataObj;

  //! Flag to determine if this instance of the object can be rendered in 3D
  bool m_canRender3D;

  //! There are multiple possible 2D views so a Hash is needed
  /*!
    The hash is indexed by the name of the 2D view.
    */
  QHash<QString, vtkProp*> m_pipe2D;

  QTreeWidgetItem* m_treeItem;

  //! True if the variable is being rendered in a 3D window.
  QList<bool> m_visible3D;

  //! The selected prop for this object.
  /*!
    The variable contains a pointer to the prop from this object that has been selected. If no prop from this object is selected then this variable is NULL. Note that if props from other objects are selected then this variable is still NULL.
    */
  vtkProp* m_selectedProp;

  QDateTime m_lastUpdate;

  //! A list of objects that interactions should be synchronized to
  QList<rtRenderObject*> m_syncList;

 private:
  //! The name of the render object
  QString m_renderName;
  //! The type of the render object
  QString m_objType;
};

#endif
