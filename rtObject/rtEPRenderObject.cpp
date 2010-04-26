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
#include "rtEPRenderObject.h"
#include "rtEPDataObject.h"

#include "rtApplication.h"
#include "rtMessage.h"

rtEPRenderObject::rtEPRenderObject()
    : m_pointMapper(0), m_meshMapper(0), m_pointActor(0), m_meshActor(0)
{
  setObjectType(rtConstants::OT_EPMesh);
  setName("EP Renderer");

  m_pointMapper = vtkPolyDataMapper::New();
  m_meshMapper = vtkPolyDataMapper::New();
  m_infoMapper = vtkPolyDataMapper::New();
  m_pointActor = vtkActor::New();
  m_meshActor = vtkActor::New();
  m_infoActor = vtkActor::New();

  setupDataObject();
  setupPipeline();
}


rtEPRenderObject::~rtEPRenderObject() {
  if(m_pointMapper) {
    m_pointMapper->Delete();
    m_pointMapper = NULL;
  }
  if(m_meshMapper) {
    m_meshMapper->Delete();
    m_meshMapper = NULL;
  }
  if(m_infoMapper) {
    m_infoMapper->Delete();
    m_infoMapper = NULL;
  }
  if(m_pointActor) {
    m_pointActor->Delete();
    m_pointActor = NULL;
  }
  if(m_meshActor) {
    m_meshActor->Delete();
    m_meshActor = NULL;
  }
  if (m_infoActor) {
    m_infoActor->Delete();
    m_infoActor = NULL;
  }
}

//! Take info from the data object. 
void rtEPRenderObject::update() {
  rtEPDataObject *dObj = static_cast<rtEPDataObject*>(m_dataObj);

  if (!dObj) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Update fialed! Data object is NULL."));
    return;
  }

  if (dObj->getPointData()) {
    dObj->getPointData()->Update();
    m_pointMapper->SetInput(dObj->getPointData());
  }
  vtkProperty *temp;
  temp = dObj->getPointProperty();
  if (temp) m_pointActor->SetProperty(temp);
  temp = dObj->getMeshProperty();
  if (temp) m_meshActor->SetProperty(temp);

  if (dObj->getMeshData()) {
    dObj->getMeshData()->Update();
    m_meshMapper->SetInput(dObj->getMeshData());
  }

  if (dObj->getColorFunction()) {
    m_meshMapper->SetLookupTable(dObj->getColorFunction());
    m_pointMapper->SetLookupTable(dObj->getColorFunction());
    m_infoMapper->SetLookupTable(dObj->getColorFunction());
  }

  if (dObj->showInfoPoints()) {
    m_infoMapper->SetInput(dObj->getInfoPolyData());
    m_infoActor->VisibilityOn();
  } else {
    m_infoActor->VisibilityOff();
  }

}

//! Add this object to the given renderer.
bool rtEPRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("addToRenderer failed! Renderer object is NULL."));
    return false;
  }

  if (!ren->HasViewProp(m_pointActor)) {
    ren->AddViewProp(m_pointActor);
  }

  if (!ren->HasViewProp(m_meshActor)) {
    ren->AddViewProp(m_meshActor);
  }

  if (!ren->HasViewProp(m_infoActor)) {
    ren->AddViewProp(m_infoActor);
  }

  setVisible3D(true);
  return true;
}

//! Remove this object from the given renderer.
bool rtEPRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);

  if (ren->HasViewProp(m_pointActor)) {
    ren->RemoveViewProp(m_pointActor);
  }

  if (ren->HasViewProp(m_meshActor)) {
    ren->RemoveViewProp(m_meshActor);
  }

  if (ren->HasViewProp(m_infoActor)) {
    ren->RemoveViewProp(m_infoActor);
  }

  return true;
}

//! Create the correct data object.
void rtEPRenderObject::setupDataObject() {
  m_dataObj = new rtEPDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtEPRenderObject::setupPipeline() {
  m_pointActor->SetMapper(m_pointMapper);
  m_meshActor->SetMapper(m_meshMapper);
  m_infoActor->SetMapper(m_infoMapper);

  m_pointMapper->SetScalarModeToUsePointData();
  m_pointMapper->UseLookupTableScalarRangeOn();
  m_pointMapper->SetColorModeToMapScalars();

  m_meshMapper->SetScalarModeToUsePointData();
  m_meshMapper->UseLookupTableScalarRangeOn();
  m_meshMapper->SetColorModeToMapScalars();

  m_infoMapper->SetScalarModeToUsePointData();
  m_infoMapper->UseLookupTableScalarRangeOn();
  m_infoMapper->SetColorModeToMapScalars();

  m_pipe3D.push_back(m_pointActor);
  m_pipe3D.push_back(m_meshActor);
  m_pipe3D.push_back(m_infoActor);
}


bool rtEPRenderObject::getObjectLocation(double loc[6]) {
  if (!m_pointActor) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get object location. Object actor is NULL."));
    return false;
  }

  m_pointActor->GetBounds(loc);
  return true;
}


