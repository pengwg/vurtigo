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

#include <vtkProp.h>
#include <vtkPropAssembly.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "objTypes.h"
class rtDataObject;
class rtMainWindow;

class rtRenderObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtRenderObject();

  virtual QList<vtkProp*>* get3DPipeline();
  virtual QHash<QString, vtkProp*>* get2DPipeline();
  rtDataObject* getDataObject();
  QString getName();
  rtConstants::rtObjectType getObjectType();

  QList<QString> get2DViewNameList() { return m_pipe2D.keys(); }
  bool viewWithNameExists(QString name) { return m_pipe2D.contains(name); }
  vtkProp* get2DViewWithName(QString name) { return m_pipe2D.value(name); }

  void setDataObject(rtDataObject* dataObj);
  void setName(QString renName);

  QTreeWidgetItem* getTreeItem() { return m_treeItem; }
  virtual void updateTreeItem();

  void setMainWindow(rtMainWindow* mainWin) { m_mainWin = mainWin; }


  void setVisible3D(bool v);
  bool getVisible3D(){ return m_visible3D; }

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren) = 0;

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren) = 0;

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

 public slots:
  virtual void mousePressEvent(QMouseEvent* event) {  }
  virtual void mouseMoveEvent(QMouseEvent* event) {  }
  virtual void mouseReleaseEvent(QMouseEvent* event) {  }
  virtual void mouseDoubleClickEvent(QMouseEvent* event) {  }
  //! Called when a key is pressed.
  virtual void keyPressEvent(QKeyEvent* event) {}
  //! Called when a key is released.
  virtual void keyReleaseEvent(QKeyEvent* event) {}
  //! The mouse wheel was moved.
  virtual void wheelEvent(QWheelEvent* event) {}


 protected:
  //! The rtRenderObject constructor.
  /*!
    The protected rtRenderObject constructor. The rtRenderObject class is not meant to be instantiated. The proper use of this class is to derive from it a specific type of renderer. 
  */
  rtRenderObject();

  void setObjectType(rtConstants::rtObjectType objType);

  //! Apply changes from the modified data object.
  virtual void update() = 0;

  //! Pointer to the data object
  rtDataObject* m_dataObj;

  //! A list of props that are part of the 3D pipeline
  QList<vtkProp*> m_pipe3D;

  //! There are multiple possible 2D views so a Hash is needed
  /*!
    The hash is indexed by the name of the 2D view.
    */
  QHash<QString, vtkProp*> m_pipe2D;

  QTreeWidgetItem* m_treeItem;
  rtMainWindow* m_mainWin;

  //! True if the variable is being rendered in the 3D window.
  bool m_visible3D;

  //! The selected prop for this object.
  /*!
    The variable contains a pointer to the prop from this object that has been selected. If no prop from this object is selected then this variable is NULL. Note that if props from other objects are selected then this variable is still NULL.
    */
  vtkProp* m_selectedProp;

  QDateTime m_lastUpdate;

 private:
  QString m_renderName;
  rtConstants::rtObjectType m_objType;
};

#endif
