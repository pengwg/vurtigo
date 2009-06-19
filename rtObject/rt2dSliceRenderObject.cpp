#include "rt2dSliceRenderObject.h"
#include "rt2dSliceDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"

#include "vtkRenderWindow.h"
#include "vtkActor.h"

rt2DSliceRenderObject::rt2DSliceRenderObject() {
  setObjectType(rtConstants::OT_2DObject);
  setName("2DSlice Renderer");
  setupDataObject();
  setupPipeline();
}


rt2DSliceRenderObject::~rt2DSliceRenderObject() {
  if (m_texturePlane) m_texturePlane->Delete();
  if (m_planeMapper) m_planeMapper->Delete();
  if (m_textureActor) m_textureActor->Delete();
  if (m_texture) m_texture->Delete();
  if (m_imgMapToColors) m_imgMapToColors->Delete();
  if (m_lookupTable) m_lookupTable->Delete();

  if (m_outlinePolyData) m_outlinePolyData->Delete();
  if (m_outlineMapper) m_outlineMapper->Delete();
  if (m_outlineActor) m_outlineActor->Delete();
  if (m_outlineProperty) m_outlineProperty->Delete();
}


//! Take info from the data object.
void rt2DSliceRenderObject::update() {
  if (!updateNeeded()) return;
  resetUpdateTime();


}

//! Add this object to the given renderer.
bool rt2DSliceRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);

  m_imgMapToColors->SetInput(dObj->getUCharData());

  if(!ren->HasViewProp(m_textureActor)) {
    ren->AddViewProp(m_textureActor);
  }

  return true;
}

//! Remove this object from the given renderer.
bool rt2DSliceRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);

  if(ren->HasViewProp(m_textureActor)) {
    ren->RemoveViewProp(m_textureActor);
  }
  return true;
}

//! Create the correct data object.
void rt2DSliceRenderObject::setupDataObject() {
  m_dataObj = new rt2DSliceDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt2DSliceRenderObject::setupPipeline() {
  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);

  // Objects for the texture pipeline.
  m_texturePlane = vtkPlaneSource::New();
  m_planeMapper = vtkPolyDataMapper::New();
  m_textureActor = vtkActor::New();
  m_texture = vtkTexture::New();
  m_imgMapToColors = vtkImageMapToColors::New();
  m_lookupTable = vtkLookupTable::New();

  m_planeMapper->SetInput(m_texturePlane->GetOutput());
  m_textureActor->SetMapper(m_planeMapper);
  m_textureActor->SetTexture(m_texture);
  m_texture->SetInputConnection(m_imgMapToColors->GetOutputPort());
  m_texture->SetLookupTable(m_lookupTable);
  m_imgMapToColors->SetLookupTable(m_lookupTable);

  // Objects for the frame outline pipeline.
  m_outlinePolyData = vtkPolyData::New();
  m_outlineMapper = vtkPolyDataMapper::New();
  m_outlineActor = vtkActor::New();
  m_outlineProperty = vtkProperty::New();

  m_pipe3D->AddPart( m_textureActor );
  m_pipe3D->AddPart( m_outlineActor );

}
