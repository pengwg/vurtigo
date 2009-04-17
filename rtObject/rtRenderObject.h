#ifndef RT_RENDER_OBJECT_H
#define RT_RENDER_OBJECT_H

#include <QString>
#include <QTreeWidgetItem>
#include <QObject>
#include <QDateTime>
#include <vtkProp.h>

#include "objTypes.h"
class rtDataObject;
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

  void setMainWindow(rtMainWindow* mainWin) { m_mainWin = mainWin; }


  void setVisible3D(bool v);
  bool getVisible3D(){ return m_visible3D; }

  //! Apply changes from the modified data object.
  virtual void update() = 0;

  void resetUpdateTime();
  bool updateNeeded();

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
  vtkProp *m_pipe3D;
  vtkProp *m_pipe2D;
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
