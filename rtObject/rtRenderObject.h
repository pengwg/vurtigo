#ifndef RT_RENDER_OBJECT_H
#define RT_RENDER_OBJECT_H

#include <QString>
#include <QTreeWidgetItem>
#include <QObject>
#include <vtkProp.h>
#include "rtDataObject.h"

class rtMainWindow;

class rtRenderObject : public QObject {

Q_OBJECT

 public:
  //! Destructor
  ~rtRenderObject();

  virtual vtkProp* get3DPipeline();
  virtual vtkProp* get2DPipeline();
  rtDataObject* getDataObject();
  QString getName();
  rtConstants::rtObjectType getObjectType();


  void setDataObject(rtDataObject* dataObj);
  void setName(QString renName);

  QTreeWidgetItem* getTreeItem() { return m_treeItem; }
  virtual void updateTreeItem();

  void initCommLinks();

  //! Invokes an updateEvent.
  void forceUpdate() { emit updateEvent(); }

  void setMainWindow(rtMainWindow* mainWin) { m_mainWin = mainWin; }

 public slots:
  //! Bring info from modified data object.
  virtual void update() = 0;
  //! Brings info from render options GUI.
  virtual void apply() = 0;

 signals:
  void updateEvent();

 protected:
  //! The rtRenderObject constructor.
  /*!
    The protected rtRenderObject constructor. The rtRenderObject class is not meant to be instantiated. The proper use of this class is to derive from it a specific type of renderer. 
  */
  rtRenderObject();

  void set3DPipeline(vtkProp* pipe3D);
  void set2DPipeline(vtkProp* pipe2D);
  void setObjectType(rtConstants::rtObjectType objType);

  rtDataObject* m_dataObj;
  rtRenderOptions* m_renderObj;
  vtkProp *m_pipe3D;
  vtkProp *m_pipe2D;
  QTreeWidgetItem* m_treeItem;
  rtMainWindow* m_mainWin;

 private:
  QString m_renderName;
  rtConstants::rtObjectType m_objType;
};

#endif
