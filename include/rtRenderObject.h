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

 public slots:
  //! Called when one of the mouse buttons is pressed.
  virtual void mousePressEvent(QMouseEvent* event,int window) {  }
  //! Called when the mouse position changes and at least one button is pressed. (See QMouseEvent)
  virtual void mouseMoveEvent(QMouseEvent* event,int window) {  }
  //! Called when a mouse button is released
  virtual void mouseReleaseEvent(QMouseEvent* event,int window) {  }
  //! Called when the mouse button is double clicked.
  virtual void mouseDoubleClickEvent(QMouseEvent* event,int window) {  }
  //! Called when a key is pressed.
  virtual void keyPressEvent(QKeyEvent* event,int window) {}
  //! Called when a key is released.
  virtual void keyReleaseEvent(QKeyEvent* event,int window) {}
  //! The mouse wheel was moved.
  virtual void wheelEvent(QWheelEvent* event,int window) {}

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
