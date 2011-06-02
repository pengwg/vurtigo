/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "rtCathRenderObject.h"
#include "rtCathDataObject.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include <iostream>
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "customQVTKWidget.h"
#include "rtColorFuncDataObject.h"

rtCathRenderObject::rtCathRenderObject() {
  setObjectType("OT_Cath");
  setName("Cath Renderer");

  setupDataObject();
  setupPipeline();
}


rtCathRenderObject::~rtCathRenderObject() {
  // Splines
  for (int ix1=0; ix1<3; ix1++) {
    if (m_spline[ix1]) m_spline[ix1]->Delete();
  }

  // Spheres Pipeline
  for (int ix1=0 ; ix1<m_sphereActor.size(); ix1++) {
      if (m_sphereActor[ix1]) m_sphereActor[ix1]->Delete();
  }
  if (m_sphere) m_sphere->Delete();
  if (m_sphereMapper) m_sphereMapper->Delete();
}


//! Take info from the data object. 
/*!
  This function is called when there is new info available in the Data part of the object and the pipeline needs to be re-done to reflect those changes.
  */
void rtCathRenderObject::update() {
  rtCathDataObject* dObj = static_cast<rtCathDataObject*>(m_dataObj);
  if (!dObj) return;

  int numLoc = dObj->getNumLocations();

  // Check if there are no locations.
  if (numLoc == 0) {
    // Empty Catheter. Just skip the rendering by turning off all visibility.
    visibilityOff();
    return;
  }

    vtkActor *tempA;
  if (m_sphereActor.size()!=numLoc) {
    // Number of locations has changed. So delete the old list and add a new one.
    while (!m_sphereActor.empty()) {
      tempA = m_sphereActor.takeFirst();
      for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
          rtApplication::instance().getMainWinHandle()->removeRenderItem(tempA,ix1);
      tempA->Delete();
    }
    m_sphereActor.clear();

    for (int ix1=0; ix1<numLoc; ix1++) {

      vtkActor *actor = vtkActor::New();
      actor->SetMapper(m_sphereMapper);
      for (int ix2=0; ix2<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix2++)
      {
          if (getVisible3D().at(ix2))
              rtApplication::instance().getMainWinHandle()->addRenderItem(actor,ix2);
      }
      m_sphereActor.append(actor);

    }
  }

  m_sphere->SetRadius( ((double)dObj->getPointSize())/10.0f );

  double val;
  int loc;
  double coords[3];

  // Get a value for the SNR
  if ( numLoc>0 ) {
    loc = dObj->getLocationList().value(0);
    dObj->getPositionAtLocation(loc, coords);
    dObj->getValueAtLocation(loc, dObj->getCurrProperty(), val);
  }

  double color[3];

  if (numLoc == 1) {
    // Just one location. Render just a ball and no spline or anything else.
    // The the first and only sphere.
    m_sphereActor[0]->SetPosition(coords);

    if (dObj->getCurrProperty() != "None" && dObj->getCurrColor() != NULL) {
        m_sphereActor[0]->SetProperty(vtkProperty::New());
        dObj->getColorAtLocation(dObj->getLocationList().value(0), dObj->getCurrProperty(),color);
        m_sphereActor[0]->GetProperty()->SetColor(color);
    } else {
        //refresh back to user given properties
        m_sphereActor[0]->SetProperty( dObj->getPointProperty() );
        dObj->refreshProperties();
    }


    // Just the spheres need to be rendered so turn everythign off and then turn the spheres back on.
    visibilityOff();
    for (int ix1=0; ix1<m_sphereActor.size(); ix1++)
        m_sphereActor[ix1]->VisibilityOn();

  } else if (numLoc > 1) {
    // Multiple locations. Render everyting.
    double direc[3];
    m_cathArrow.setConeCenter(coords);
    m_cathArrow.setConeRadius(2.0f);
    m_cathArrow.setConeHeight(2.0f);
    m_cathArrow.setConeResolution(50);

    loc = dObj->getLocationList().value(1);
    dObj->getPositionAtLocation(loc, direc);

    m_cathArrow.setConeDirection(coords[0]-direc[0], coords[1]-direc[1], coords[2]-direc[2]);

    for (int ix2=0; ix2<3; ix2++) {
      // Remove old points
      m_spline[ix2]->RemoveAllPoints();

      // Set the new tension.
      m_spline[ix2]->SetDefaultTension(dObj->getTension());

      // Set the new continuity parameter.
      m_spline[ix2]->SetDefaultContinuity(dObj->getContinuity());

      // Set the left and right values for the spline.
      // This sets the curvature of the spline at the endpoints.
      m_spline[ix2]->SetLeftValue(dObj->getTipValue());
      m_spline[ix2]->SetRightValue(dObj->getEndValue());
    }

    for (int ix1=0; ix1<numLoc; ix1++) {
      loc = dObj->getLocationList().value(ix1);
      dObj->getPositionAtLocation(loc, coords);
      dObj->getValueAtLocation(loc, dObj->getCurrProperty(), val);


      m_sphereActor[ix1]->SetPosition(coords);

      if (dObj->getCurrProperty() != "None" && dObj->getCurrColor() != NULL) {
          m_sphereActor[ix1]->SetProperty(vtkProperty::New());
          dObj->getColorAtLocation(loc, dObj->getCurrProperty(),color);
          m_sphereActor[ix1]->GetProperty()->SetColor(color);
      } else {
          //refresh back to user given properties
          m_sphereActor[ix1]->SetProperty( dObj->getPointProperty() );
          dObj->refreshProperties();
      }


      for (int ix2=0; ix2<3; ix2++) {
        m_spline[ix2]->AddPoint(ix1,coords[ix2]);
      }
    }

    m_spline[0]->Compute();
    m_spline[1]->Compute();
    m_spline[2]->Compute();

    m_cathArrow.clearPointList();
    m_cathArrow.setNumPoints(40);
    for (int ix1=0; ix1<m_cathArrow.getNumPoints(); ix1++) {
      m_cathArrow.setPoint(ix1,
                               m_spline[0]->Evaluate( ((double)numLoc-1.0f)/((double)m_cathArrow.getNumPoints())*(double)ix1 ),
                               m_spline[1]->Evaluate( ((double)numLoc-1.0f)/((double)m_cathArrow.getNumPoints())*(double)ix1 ),
                               m_spline[2]->Evaluate( ((double)numLoc-1.0f)/((double)m_cathArrow.getNumPoints())*(double)ix1 ));
    }

    m_cathArrow.setTubeThickness(dObj->getSplineThickness());
    visibilityOn();
  }
}

//! Add this object to the given renderer.
bool rtCathRenderObject::addToRenderer(vtkRenderer* ren, int window) {
  if (!ren) return false;
  setVisible3D(window,true);

  for (int ix1=0; ix1<m_sphereActor.size(); ix1++)
  {
      if (!ren->HasViewProp(m_sphereActor[ix1])) {
          ren->AddViewProp(m_sphereActor[ix1]);
      }
  }
  if (!ren->HasViewProp(m_cathArrow.getTubeActor())) {
    ren->AddViewProp(m_cathArrow.getTubeActor());
  }
  if (!ren->HasViewProp(m_cathArrow.getConeActor())) {
    ren->AddViewProp(m_cathArrow.getConeActor());
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

  return true;
}

//! Remove this object from the given renderer.
bool rtCathRenderObject::removeFromRenderer(vtkRenderer* ren, int window) {
  if (!ren) return false;
  setVisible3D(window,false);

  for (int ix1=0; ix1<m_sphereActor.size(); ix1++)
  {
      if (ren->HasViewProp(m_sphereActor[ix1])) {
          ren->RemoveViewProp(m_sphereActor[ix1]);
      }
  }
  if (ren->HasViewProp(m_cathArrow.getTubeActor())) {
    ren->RemoveViewProp(m_cathArrow.getTubeActor());
  }
  if (ren->HasViewProp(m_cathArrow.getConeActor())) {
    ren->RemoveViewProp(m_cathArrow.getConeActor());
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
void rtCathRenderObject::setupDataObject() {
  m_dataObj = new rtCathDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtCathRenderObject::setupPipeline() {
  rtCathDataObject* dObj = static_cast<rtCathDataObject*>(m_dataObj);

  // Spline Pipeline
  m_cathArrow.setNumPoints(40);
  m_cathArrow.getTubeActor()->SetProperty(dObj->getSplineProperty());
  m_cathArrow.getConeActor()->SetProperty(dObj->getTipProperty());

  // Create the x,y,z splines
  for (int ix1=0; ix1<3; ix1++) {
    m_spline[ix1] = vtkKochanekSpline::New();
    m_spline[ix1]->SetLeftConstraint(1);
    m_spline[ix1]->SetRightConstraint(1);
  }

  m_canRender3D = true;

  m_sphere = vtkSphereSource::New();
  m_sphere->SetThetaResolution(50);
  m_sphere->SetPhiResolution(50);
  m_sphereMapper = vtkPolyDataMapper::New();
  m_sphereMapper->SetInputConnection(m_sphere->GetOutputPort());

  // But set their visibility to off
  visibilityOff();
}

//! The position of the catheter tip is returned.
bool rtCathRenderObject::getObjectLocation(double loc[6]) {
  if (!m_sphereActor[0]) return false;

  m_sphereActor[0]->GetBounds(loc);

  return true;
}

void rtCathRenderObject::visibilityOn() {
  for (int ix1=0; ix1<m_sphereActor.size(); ix1++)
      m_sphereActor[ix1]->VisibilityOn();
  m_cathArrow.tubeVisibilityOn();
  m_cathArrow.coneVisibilityOn();
}

void rtCathRenderObject::visibilityOff() {
  for (int ix1=0; ix1<m_sphereActor.size(); ix1++)
      m_sphereActor[ix1]->VisibilityOff();
  m_cathArrow.tubeVisibilityOff();
  m_cathArrow.coneVisibilityOff();
}
