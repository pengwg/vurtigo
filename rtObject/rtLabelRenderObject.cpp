#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType("OT_TextLabel");
  setName("Simple Label Renderer");
  setupDataObject();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
  if (m_textActor2D) m_textActor2D->Delete();
}


//! Take the info from the DataObject
void rtLabelRenderObject::update() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());

  if (rtApplication::instance().getMainWinHandle() && m_textActor2D->GetNumberOfConsumers() > 0) {
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

//! Add this object to the given renderer.
bool rtLabelRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,true);
  if (ren->HasViewProp(m_textActor2D)) return false;
  ren->AddViewProp(m_textActor2D);
  return true;
}

//! Remove this object from the given renderer.
bool rtLabelRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,false);
  if (!ren->HasViewProp(m_textActor2D)) return false;
  ren->RemoveViewProp(m_textActor2D);
  return true;
}

void rtLabelRenderObject::setupDataObject() {
  m_dataObj = new rtLabelDataObject();
  
}

void rtLabelRenderObject::setupPipeline() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D = vtkTextActor::New();
  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());
  m_textActor2D->SetTextScaleModeToNone();

  m_canRender3D = true;

  m_pipe2D.insert("Label", m_textActor2D);
}

//! This object has no valid 3D location.
bool rtLabelRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
