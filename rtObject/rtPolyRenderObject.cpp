#include "rtPolyRenderObject.h"
#include "rtPolyDataObject.h"

rtPolyRenderObject::rtPolyRenderObject() {
  setObjectType(rtConstants::OT_vtkPolyData);
  setName("Poly Renderer");
  setupDataObject();
  setupPipeline();
}


rtPolyRenderObject::~rtPolyRenderObject() {
  m_actor->Delete();
  m_mapper->Delete();
}

//! Take info from the data object. 
void rtPolyRenderObject::update() {
  rtPolyDataObject *dObj = static_cast<rtPolyDataObject*>(m_dataObj);

  if (!dObj) return;

  dObj->getPolyData()->Update();

  m_mapper->SetLookupTable(dObj->getColorTable());
  m_actor->SetProperty(dObj->getProperty());
  m_mapper->SetInput(dObj->getPolyData());
}

//! Add this object to the given renderer.
bool rtPolyRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;

  if (!ren->HasViewProp(m_actor)) {
    ren->AddViewProp(m_actor);
  }

  return true;
}

//! Remove this object from the given renderer.
bool rtPolyRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;

  if (ren->HasViewProp(m_actor)) {
    ren->RemoveViewProp(m_actor);
  }

  return true;
}

//! Create the correct data object.
void rtPolyRenderObject::setupDataObject() {
  m_dataObj = new rtPolyDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtPolyRenderObject::setupPipeline() {
  rtPolyDataObject *dObj = static_cast<rtPolyDataObject*>(m_dataObj);
  m_actor = vtkActor::New();
  m_mapper = vtkPolyDataMapper::New();

  m_actor->SetMapper(m_mapper);
  m_actor->SetProperty(dObj->getProperty());

  m_mapper->SetInput(dObj->getPolyData());
  m_mapper->SetScalarModeToUsePointData();
  m_mapper->UseLookupTableScalarRangeOn();\
  m_mapper->SetColorModeToMapScalars();
  m_mapper->SetLookupTable(dObj->getColorTable());

  m_pipe3D.push_back(m_actor);
}
