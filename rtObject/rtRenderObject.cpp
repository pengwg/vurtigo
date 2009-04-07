#include "rtRenderObject.h"

rtRenderObject::rtRenderObject() {
  m_pipe3D = NULL;
  m_pipe2D = NULL;
  m_dataObj = NULL;
  m_renderName = "None";
  m_objType = rtConstants::OT_None;
  m_treeItem = new QTreeWidgetItem();
  m_mainWin = NULL;
}

rtRenderObject::~rtRenderObject() {
  if (m_treeItem) delete m_treeItem;
}

vtkProp* rtRenderObject::get3DPipeline() {
  return m_pipe3D;
}

vtkProp* rtRenderObject::get2DPipeline() {
  return m_pipe2D;
}

rtDataObject* rtRenderObject::getDataObject() {
  return m_dataObj;
}

QString rtRenderObject::getName() {
  return m_renderName;
}

rtConstants::rtObjectType rtRenderObject::getObjectType() {
  return m_objType;
}

void rtRenderObject::set3DPipeline(vtkProp* pipe3D){
  m_pipe3D = pipe3D;
}

void rtRenderObject::set2DPipeline(vtkProp* pipe2D) {
  m_pipe2D = pipe2D;
}

void rtRenderObject::setDataObject(rtDataObject* dataObj) {
  m_dataObj = dataObj;
}

void rtRenderObject::setName(QString renName) {
  m_renderName = renName;
}

void rtRenderObject::setObjectType(rtConstants::rtObjectType objType) {
  m_objType = objType;
}


void rtRenderObject::updateTreeItem() {
  if (m_treeItem) {
    m_treeItem->setText(0, m_dataObj->getObjName());
    m_treeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_treeItem->setText(1, QString::number(m_dataObj->getId()));
    m_treeItem->setTextAlignment(1, Qt::AlignHCenter);
    if (m_pipe3D) {
      m_treeItem->setText(2, "3D");
      m_treeItem->setCheckState(2,Qt::Unchecked); 
    } else {
      m_treeItem->setText(2, "NA");
    }
  }
}

//! Setup all the signals and slots. 
/*!
  This function will create all the connections between the three parts that make an object. 
  The way this is done is through update() and apply(). The update() call will use new data in the rtDataObject and propagate that to the other objects. The apply call will use the user input from the GUI (rtRenderOptions) and propagate that to the other objects. This function is called by the rtObjectManager after the object has been created. 
 */
void rtRenderObject::initCommLinks() {
  connect(m_renderObj->getApplyButton(), SIGNAL(pressed()), this, SLOT(apply()));
  connect(m_renderObj->getApplyButton(), SIGNAL(pressed()), m_dataObj, SLOT(apply()));
  connect(m_renderObj->getApplyButton(), SIGNAL(pressed()), m_renderObj, SLOT(apply()));

  connect(this, SIGNAL(updateEvent()), this, SLOT(update()));
  connect(this, SIGNAL(updateEvent()), m_dataObj, SLOT(update()));
  connect(this, SIGNAL(updateEvent()), m_renderObj, SLOT(update()));
}
