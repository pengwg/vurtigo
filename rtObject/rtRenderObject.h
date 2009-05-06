#ifndef RT_RENDER_OBJECT_H
#define RT_RENDER_OBJECT_H

#include <QString>
#include <QTreeWidgetItem>
#include <QObject>
#include <QDateTime>
#include <QList>
#include <vtkProp.h>
#include <vtkPropAssembly.h>

#include "objTypes.h"
class rtDataObject;
class rtMainWindow;

class rtRenderObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtRenderObject();

  virtual vtkPropAssembly* get3DPipeline();
  virtual vtkPropAssembly* get2DPipeline();
  rtDataObject* getDataObject();
  QString getName();
  rtConstants::rtObjectType getObjectType();


  void setDataObject(rtDataObject* dataObj);
  void setName(QString renName);

  QTreeWidgetItem* getTreeItem() { return m_treeItem; }
  virtual void updateTreeItem();

  void setMainWindow(rtMainWindow* mainWin) { m_mainWin = mainWin; }


  void setVisible3D(bool v);
  bool getVisible3D(){ return m_visible3D; }

  //! Apply changes from the modified data object.
  virtual void update() = 0;

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
  vtkPropAssembly* m_pipe2D;
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
