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

#include "objTypes.h"
class rtDataObject;
class rtMainWindow;

class rtRenderObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtRenderObject();

  virtual vtkPropAssembly* get3DPipeline();
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

  //! Apply changes from the modified data object.
  virtual void update() = 0;

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren) = 0;

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren) = 0;

  void resetUpdateTime();
  bool updateNeeded();

  virtual bool mousePressEvent(QMouseEvent* event) { return false; }
  virtual bool mouseMoveEvent(QMouseEvent* event) { return false; }
  virtual bool mouseReleaseEvent(QMouseEvent* event) { return false; }
  virtual bool mouseDoubleClickEvent(QMouseEvent* event) { return false; }

 protected:
  //! The rtRenderObject constructor.
  /*!
    The protected rtRenderObject constructor. The rtRenderObject class is not meant to be instantiated. The proper use of this class is to derive from it a specific type of renderer. 
  */
  rtRenderObject();

  void setObjectType(rtConstants::rtObjectType objType);

  rtDataObject* m_dataObj;
  vtkPropAssembly* m_pipe3D;

  //! There are multiple possible 2D views so a Hash is needed
  /*!
    The hash is indexed by the name of the 2D view.
    */
  QHash<QString, vtkProp*> m_pipe2D;

  QTreeWidgetItem* m_treeItem;
  rtMainWindow* m_mainWin;

  //! True if the variable is being rendered in the 3D window.
  bool m_visible3D;

  QDateTime m_lastUpdate;

 private:
  QString m_renderName;
  rtConstants::rtObjectType m_objType;
};

#endif
