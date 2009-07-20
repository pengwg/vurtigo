#include "rtCathRenderObject.h"
#include "rtCathDataObject.h"
#include <iostream>

rtCathRenderObject::rtCathRenderObject() {
  setObjectType(rtConstants::OT_Cath);
  setName("Cath Renderer");

  setupDataObject();
  setupPipeline();
}


rtCathRenderObject::~rtCathRenderObject() {
  // Spline Pipeline
  if (m_splineActor) m_splineActor->Delete();
  if (m_splineMapper) m_splineMapper->Delete();
  if (m_splineFilter) m_splineFilter->Delete();
  if (m_splineLineData) m_splineLineData->Delete();
  if (m_splineCellArray) m_splineCellArray->Delete();
  if (m_splinePoints) m_splinePoints->Delete();

  for (int ix1=0; ix1<3; ix1++) {
    if (m_spline[ix1]) m_spline[ix1]->Delete();
  }

  // Spheres Pipeline
  for (int ix1=0 ; ix1<m_sphereList.size(); ix1++) {
    if (m_sphereList[ix1]) m_sphereList[ix1]->Delete();
  }
  if (m_sphereAppend) m_sphereAppend->Delete();
  if (m_sphereMapper) m_sphereMapper->Delete();
  if (m_sphereActor) m_sphereActor->Delete();

  // Cath Tip Pipeline
  if (m_coneSource) m_coneSource->Delete();
  if (m_coneMapper) m_coneMapper->Delete();
  if (m_coneActor) m_coneActor->Delete();
}


//! Take info from the data object. 
/*!
  This function is called when there is new info available in the Data part of the object and the pipeline needs to be re-done to reflect those changes.
  */
void rtCathRenderObject::update() {
  rtCathDataObject* dObj = static_cast<rtCathDataObject*>(m_dataObj);
  if (!dObj) return;

  // The catheter is rendered differently depending on the number of locations.
  vtkSphereSource* temp;

  int numLoc = dObj->getNumLocations();

  // Check if there are no locations.
  if (numLoc == 0) {
    // Empty Catheter. Just skip the rendering by turning off all visibility.
    if(m_splineActor) m_splineActor->VisibilityOff();
    if(m_sphereActor) m_sphereActor->VisibilityOff();
    if(m_coneActor) m_coneActor->VisibilityOff();
    return;
  }

  if (m_sphereList.size()!=numLoc) {
    // Number of locations has changed. So delete the old list and add a new one.
    while (!m_sphereList.empty()) {
      temp = m_sphereList.takeFirst();
      m_sphereAppend->RemoveInput(temp->GetOutput());
      temp->Delete();
    }
    for (int ix1=0; ix1<numLoc; ix1++) {
      temp = vtkSphereSource::New();
      m_sphereList.append(temp);
      m_sphereAppend->AddInput(temp->GetOutput());
    }
  }

  if (numLoc == 1) {
    // Just one location. Render just a ball and no spline or anything else.
    int loc;
    double coords[3];
    double SNR;

    loc = dObj->getLocationList().value(0);
    dObj->getPositionAtLocation(loc, coords);
    dObj->getSNRAtLocation(loc, SNR);

    // The the first and only sphere.
    temp = m_sphereList.value(0);
    temp->SetCenter(coords);
    temp->SetRadius(10.0f/SNR);
    temp->SetThetaResolution(50);
    temp->SetPhiResolution(50);

    m_sphereActor->VisibilityOn();
    m_splineActor->VisibilityOff();
    m_coneActor->VisibilityOff();
  } else if (numLoc > 1) {
    // Multiple locations. Render everyting.
    int loc;
    double coords[3];
    double direc[3];
    double SNR;

    // Do the tip.
    loc = dObj->getLocationList().value(0);
    dObj->getPositionAtLocation(loc, coords);
    dObj->getSNRAtLocation(loc, SNR);

    m_coneSource->SetCenter(coords);
    m_coneSource->SetRadius(25.0f/SNR);
    m_coneSource->SetHeight(30.0f/SNR);
    m_coneSource->SetResolution(80);

    loc = dObj->getLocationList().value(1);
    dObj->getPositionAtLocation(loc, direc);

    m_coneSource->SetDirection(coords[0]-direc[0], coords[1]-direc[1], coords[2]-direc[2]);

    for (int ix2=0; ix2<3; ix2++) {
      m_spline[ix2]->RemoveAllPoints();
    }

    for (int ix1=0; ix1<numLoc; ix1++) {
      loc = dObj->getLocationList().value(ix1);
      dObj->getPositionAtLocation(loc, coords);
      dObj->getSNRAtLocation(loc, SNR);

      temp = m_sphereList.value(ix1);
      temp->SetCenter(coords);
      temp->SetRadius(10.0f/SNR);
      temp->SetThetaResolution(50);
      temp->SetPhiResolution(50);

      for (int ix2=0; ix2<3; ix2++) {
        m_spline[ix2]->AddPoint(ix1,coords[ix2]);
      }
    }

    m_spline[0]->Compute();
    m_spline[1]->Compute();
    m_spline[2]->Compute();

    m_splinePoints->Reset();
    m_splinePoints->SetNumberOfPoints (m_numSplinePoints+1);
    for (int ix1=0; ix1<=m_numSplinePoints; ix1++) {
      m_splinePoints->SetPoint(ix1,
                               m_spline[0]->Evaluate( ((double)numLoc-1.0f)/((double)m_numSplinePoints)*(double)ix1 ),
                               m_spline[1]->Evaluate( ((double)numLoc-1.0f)/((double)m_numSplinePoints)*(double)ix1 ),
                               m_spline[2]->Evaluate( ((double)numLoc-1.0f)/((double)m_numSplinePoints)*(double)ix1 ));
    }

    m_splineFilter->SetRadius(5.0f/SNR);

    m_sphereActor->VisibilityOn();
    m_splineActor->VisibilityOn();
    m_coneActor->VisibilityOn();
  }

}

//! Add this object to the given renderer.
bool rtCathRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(true);
  if (!ren->HasViewProp(m_sphereActor)) {
    ren->AddViewProp(m_sphereActor);
  }
  if (!ren->HasViewProp(m_splineActor)) {
    ren->AddViewProp(m_splineActor);
  }
  if (!ren->HasViewProp(m_coneActor)) {
    ren->AddViewProp(m_coneActor);
  }
  return true;
}

//! Remove this object from the given renderer.
bool rtCathRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);
  if (ren->HasViewProp(m_sphereActor)) {
    ren->RemoveViewProp(m_sphereActor);
  }
  if (ren->HasViewProp(m_splineActor)) {
    ren->RemoveViewProp(m_splineActor);
  }
  if (ren->HasViewProp(m_coneActor)) {
    ren->RemoveViewProp(m_coneActor);
  }
  return true;
}

//! Create the correct data object.
void rtCathRenderObject::setupDataObject() {
  m_dataObj = new rtCathDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtCathRenderObject::setupPipeline() {
  rtCathDataObject* dObj = static_cast<rtCathDataObject*>(m_dataObj);


  // Spline Pipeline
  m_numSplinePoints = 40;

  m_splineActor = vtkActor::New();
  m_splineMapper = vtkPolyDataMapper::New();
  m_splineFilter = vtkTubeFilter::New();
  m_splineLineData = vtkPolyData::New();
  m_splineCellArray = vtkCellArray::New();
  m_splinePoints = vtkPoints::New();

  // Create the x,y,z splines
  for (int ix1=0; ix1<3; ix1++) {
    m_spline[ix1] = vtkKochanekSpline::New();
  }

  m_splineCellArray->InsertNextCell(m_numSplinePoints);
  for (int ix1=0; ix1<m_numSplinePoints; ix1++) {
    m_splineCellArray->InsertCellPoint(ix1);
  }

  m_splinePoints->SetDataTypeToDouble ();
  m_splineLineData->SetPoints(m_splinePoints);
  m_splineLineData->SetLines(m_splineCellArray);
  m_splineFilter->SetInput(m_splineLineData);
  m_splineFilter->SetNumberOfSides(50);
  m_splineMapper->SetInput(m_splineFilter->GetOutput());
  m_splineActor->SetMapper(m_splineMapper);
  m_splineActor->SetProperty( dObj->getSplineProperty() );

  // Spheres Pipeline
  m_sphereAppend = vtkAppendPolyData::New();
  m_sphereMapper = vtkPolyDataMapper::New();
  m_sphereActor = vtkActor::New();

  m_sphereMapper->SetInput(m_sphereAppend->GetOutput());
  m_sphereActor->SetMapper(m_sphereMapper);
  m_sphereActor->SetProperty( dObj->getPointProperty() );

  // Cath Tip Pipeline
  m_coneSource = vtkConeSource::New() ;
  m_coneMapper = vtkPolyDataMapper::New();
  m_coneActor = vtkActor::New();

  m_coneMapper->SetInput(m_coneSource->GetOutput());
  m_coneActor->SetMapper(m_coneMapper);
  m_coneActor->SetProperty( dObj->getTipProperty() );

  // Add the actors to the render list.
  m_pipe3D.push_back(m_splineActor);
  m_pipe3D.push_back(m_sphereActor);
  m_pipe3D.push_back(m_coneActor);

  // But set their visibility to off
  m_splineActor->VisibilityOff();
  m_sphereActor->VisibilityOff();
  m_coneActor->VisibilityOff();

}

//! The position of the catheter tip is returned.
bool rtCathRenderObject::getObjectLocation(double loc[6]) {
  if (!m_sphereActor) return false;

  m_sphereActor->GetBounds(loc);

  return true;
}
