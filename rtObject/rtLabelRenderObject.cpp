#include "rtLabelRenderObject.h"
#include "rtLabelRenderOptions.h"
#include "rtLabelDataObject.h"
#include "rtMainWindow.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType(rtConstants::OT_TextLabel);
  setName("Simple Label Renderer");
  setupDataObject();
  setupRenderOptions();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
  if (m_dataObj) delete m_dataObj;
  if (m_renderObj) delete m_renderObj;

  if (m_textActor2D) m_textActor2D->Delete();
}

//! Take info from the GUI
void rtLabelRenderObject::apply() {
  QString tempText;
  tempText = dynamic_cast<rtLabelRenderOptions*>(m_renderObj)->getChooseTextEdit()->text();  
  m_textActor2D->SetInput(tempText.toStdString().c_str());

  if (m_mainWin && m_pipe3D->GetNumberOfConsumers() > 0) {
    m_mainWin->setRenderFlag3D(true);
  }
}

//! Take the info from the DataObject
void rtLabelRenderObject::update() {
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

void rtLabelRenderObject::setupRenderOptions() {
  m_renderObj = new rtLabelRenderOptions();
  m_dataObj->setRenderOptions(m_renderObj);
}

void rtLabelRenderObject::setupPipeline() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D = vtkTextActor::New();
  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());

  // This should work the same way in both the 3D and 2D render windows. 
  m_pipe3D = m_textActor2D;
  m_pipe2D = m_textActor2D;
}

