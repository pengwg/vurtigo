#include "rtLabelRenderObject.h"
#include "rtLabelRenderOptions.h"
#include "rtLabelDataObject.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType(rtConstants::OT_TextLabel);
  setupDataObject();
  setupRenderOptions();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
  if (m_dataObj) delete m_dataObj;
  if (m_renderObj) delete m_renderObj;

  if (textActor2D) textActor2D->Delete();
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

  textActor2D = vtkTextActor::New();
  textActor2D->SetInput(dObj->getText().toStdString().c_str());
  textActor2D->SetTextProperty(dObj->getTextProperty());
}
