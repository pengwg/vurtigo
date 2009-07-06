#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtMainWindow.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType(rtConstants::OT_TextLabel);
  setName("Simple Label Renderer");
  setupDataObject();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
  if (m_dataObj) delete m_dataObj;
  if (m_textActor2D) m_textActor2D->Delete();
}


//! Take the info from the DataObject
void rtLabelRenderObject::update() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());

  if (m_mainWin && m_textActor2D->GetNumberOfConsumers() > 0) {
    m_mainWin->setRenderFlag3D(true);
  }
}

//! Add this object to the given renderer.
bool rtLabelRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);
  if (ren->HasViewProp(m_textActor2D)) return false;
  ren->AddViewProp(m_textActor2D);
  return true;
}

//! Remove this object from the given renderer.
bool rtLabelRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
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

  // This should work the same way in both the 3D and 2D render windows. 
  m_pipe3D.push_back(m_textActor2D);

  m_pipe2D.insert("Label", m_textActor2D);
}

