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

  if (!updateNeeded()) return;
  resetUpdateTime();

  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());

  if (m_mainWin && m_pipe3D->GetNumberOfConsumers() > 0) {
    m_mainWin->setRenderFlag3D(true);
  }
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
  m_pipe3D->AddPart(m_textActor2D);
  m_pipe2D->AddPart(m_textActor2D);
}

