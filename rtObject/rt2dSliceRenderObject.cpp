#include "rt2dSliceRenderObject.h"
#include "rt2dSliceDataObject.h"
#include "rtObjectManager.h"
#include "rtMainWindow.h"

#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkCellArray.h"

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

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if ( !dObj->isDataValid() ) return;

  m_imgMapToColors->SetInput(dObj->getUCharData());

  double scaleRange[2];
  dObj->getUCharData()->GetScalarRange(scaleRange);
  m_lookupTable->SetRange(scaleRange[0], scaleRange[1]);

  double orig[3], pt1[3], pt2[3], opp[3];
  double bounds[6];
  double xsize, ysize, zsize;

  dObj->getUCharData()->GetBounds(bounds);

  xsize = bounds[1]-bounds[0];
  ysize = bounds[3]-bounds[2];

  // The Z size should be one...
  zsize = bounds[5]-bounds[4];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*ysize; pt2[2]=0.0;
  opp[0]=1.0*xsize; opp[1]=1.0*ysize; opp[2]=0.0;

  dObj->getTransform()->TransformPoint(orig, orig);
  dObj->getTransform()->TransformPoint(pt1, pt1);
  dObj->getTransform()->TransformPoint(pt2, pt2);
  dObj->getTransform()->TransformPoint(opp, opp);

  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, orig);
  pts->SetPoint(1, pt1);
  pts->SetPoint(2, opp);
  pts->SetPoint(3, pt2);
  m_outlinePolyData->SetPoints(pts);
  pts->Delete();

  m_texturePlane->SetOrigin(orig);
  m_texturePlane->SetPoint1(pt1);
  m_texturePlane->SetPoint2(pt2);

}

//! Add this object to the given renderer.
bool rt2DSliceRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if (!dObj->isDataValid()) return false;

  // Update the object.
  dObj->Modified();
  this->update();

  if(!ren->HasViewProp(m_textureActor)) {
    ren->AddViewProp(m_textureActor);
  }

  if(!ren->HasViewProp(m_outlineActor)) {
    ren->AddViewProp(m_outlineActor);
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

  if(ren->HasViewProp(m_outlineActor)) {
    ren->RemoveViewProp(m_outlineActor);
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

  // Balck and white.
  m_imgMapToColors->SetOutputFormatToLuminance();

  m_texturePlane->SetOrigin(0,0,0);
  m_texturePlane->SetPoint1(1,0,0);
  m_texturePlane->SetPoint2(0,1,0);

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


  vtkPoints* pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  pts->SetPoint(0, 0.0, 0.0, 0.0);
  pts->SetPoint(1, 1.0, 0.0, 0.0);
  pts->SetPoint(2, 1.0, 1.0, 0.0);
  pts->SetPoint(3, 0.0, 1.0, 0.0);

  m_outlinePolyData->SetPoints(pts);
  pts->Delete();

  vtkCellArray* lines = vtkCellArray::New();
  lines->InsertNextCell(5);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);
  lines->InsertCellPoint(3);
  lines->InsertCellPoint(0);
  m_outlinePolyData->SetLines(lines);
  lines->Delete();

  m_outlineMapper->SetInput(m_outlinePolyData);
  m_outlineActor->SetMapper(m_outlineMapper);
  m_outlineActor->SetProperty(m_outlineProperty);

  m_pipe3D->AddPart( m_textureActor );
  m_pipe3D->AddPart( m_outlineActor );
}
