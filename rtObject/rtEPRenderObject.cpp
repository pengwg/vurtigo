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

rtEPRenderObject::rtEPRenderObject() {
  setObjectType(rtConstants::OT_EPMesh);
  setName("EP Renderer");
  setupDataObject();
  setupPipeline();
}


rtEPRenderObject::~rtEPRenderObject() {

}

//! Take info from the data object. 
void rtEPRenderObject::update() {
  rtEPDataObject *dObj = static_cast<rtEPDataObject*>(m_dataObj);

  if (!dObj) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Update fialed! Data object is NULL."));
    return;
  }



}

//! Add this object to the given renderer.
bool rtEPRenderObject::addToRenderer(vtkRenderer* ren) {
  if (!ren) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("addToRenderer failed! Renderer object is NULL."));
    return false;
  }

  setVisible3D(true);


  return true;
}

//! Remove this object from the given renderer.
bool rtEPRenderObject::removeFromRenderer(vtkRenderer* ren) {
  if (!ren) return false;
  setVisible3D(false);

  return true;
}

//! Create the correct data object.
void rtEPRenderObject::setupDataObject() {
  m_dataObj = new rtEPDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtEPRenderObject::setupPipeline() {

}


bool rtEPRenderObject::getObjectLocation(double loc[6]) {

  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return true;
}


