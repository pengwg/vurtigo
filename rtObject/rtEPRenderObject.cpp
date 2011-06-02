#include "rtEPRenderObject.h"
#include "rtEPDataObject.h"

#include "rtApplication.h"
#include "rtMainWindow.h"
#include "customQVTKWidget.h"
#include "rtMessage.h"

rtEPRenderObject::rtEPRenderObject()
    : m_pointMapper(0), m_meshMapper(0), m_pointActor(0), m_meshActor(0)
{
  setObjectType("OT_EPMesh");
  setName("EP Renderer");

  m_pointMapper = vtkPolyDataMapper::New();
  m_meshMapper = vtkPolyDataMapper::New();
  m_infoMapper = vtkPolyDataMapper::New();
  m_pointActor = vtkActor::New();
  m_meshActor = vtkActor::New();
  m_infoActor = vtkActor::New();

  setupDataObject();
  setupPipeline();
}


rtEPRenderObject::~rtEPRenderObject() {
  if(m_pointMapper) {
    m_pointMapper->Delete();
    m_pointMapper = NULL;
  }
  if(m_meshMapper) {
    m_meshMapper->Delete();
    m_meshMapper = NULL;
  }
  if(m_infoMapper) {
    m_infoMapper->Delete();
    m_infoMapper = NULL;
  }
  if(m_pointActor) {
    m_pointActor->Delete();
    m_pointActor = NULL;
  }
  if(m_meshActor) {
    m_meshActor->Delete();
    m_meshActor = NULL;
  }
  if (m_infoActor) {
    m_infoActor->Delete();
    m_infoActor = NULL;
  }
}

//! Take info from the data object. 
void rtEPRenderObject::update() {
  rtEPDataObject *dObj = static_cast<rtEPDataObject*>(m_dataObj);

  if (!dObj) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Update fialed! Data object is NULL."));
    return;
  }

  if (dObj->getPointData()) {
    dObj->getPointData()->Update();
    m_pointMapper->SetInput(dObj->getPointData());
  }
  vtkProperty *temp;
  temp = dObj->getPointProperty();
  if (temp) m_pointActor->SetProperty(temp);
  temp = dObj->getMeshProperty();
  if (temp) m_meshActor->SetProperty(temp);

  if (dObj->getMeshData()) {
    dObj->getMeshData()->Update();
    m_meshMapper->SetInput(dObj->getMeshData());
  }

  if (dObj->getColorFunction()) {
    m_meshMapper->SetLookupTable(dObj->getColorFunction());
    m_pointMapper->SetLookupTable(dObj->getColorFunction());
    m_infoMapper->SetLookupTable(dObj->getColorFunction());
  }

  if (dObj->showInfoPoints()) {
    m_infoMapper->SetInput(dObj->getInfoPolyData());
    m_infoActor->VisibilityOn();
  } else {
    m_infoActor->VisibilityOff();
  }

}

//! Add this object to the given renderer.
bool rtEPRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("addToRenderer failed! Renderer object is NULL."));
    return false;
  }

  if (!ren->HasViewProp(m_pointActor)) {
    ren->AddViewProp(m_pointActor);
  }

  if (!ren->HasViewProp(m_meshActor)) {
    ren->AddViewProp(m_meshActor);
  }

  if (!ren->HasViewProp(m_infoActor)) {
    ren->AddViewProp(m_infoActor);
  }

  // Connect signals and slots for interaction.
  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);
  // Connect mouse actions
  connect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));

  setVisible3D(window,true);
  return true;
}

//! Remove this object from the given renderer.
bool rtEPRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,false);

  if (ren->HasViewProp(m_pointActor)) {
    ren->RemoveViewProp(m_pointActor);
  }

  if (ren->HasViewProp(m_meshActor)) {
    ren->RemoveViewProp(m_meshActor);
  }

  if (ren->HasViewProp(m_infoActor)) {
    ren->RemoveViewProp(m_infoActor);
  }

  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));


  return true;
}

//! Create the correct data object.
void rtEPRenderObject::setupDataObject() {
  m_dataObj = new rtEPDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtEPRenderObject::setupPipeline() {
  m_pointActor->SetMapper(m_pointMapper);
  m_meshActor->SetMapper(m_meshMapper);
  m_infoActor->SetMapper(m_infoMapper);

  m_pointMapper->SetScalarModeToUsePointData();
  m_pointMapper->UseLookupTableScalarRangeOn();
  m_pointMapper->SetColorModeToMapScalars();

  m_meshMapper->SetScalarModeToUsePointData();
  m_meshMapper->UseLookupTableScalarRangeOn();
  m_meshMapper->SetColorModeToMapScalars();

  m_infoMapper->SetScalarModeToUsePointData();
  m_infoMapper->UseLookupTableScalarRangeOn();
  m_infoMapper->SetColorModeToMapScalars();

  // This pipeline can be rendered in 3D
  m_canRender3D = true;
}


bool rtEPRenderObject::getObjectLocation(double loc[6]) {
  if (!m_pointActor) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get object location. Object actor is NULL."));
    return false;
  }

  m_pointActor->GetBounds(loc);
  return true;
}



