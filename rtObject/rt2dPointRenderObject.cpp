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
#include "rt2dPointRenderObject.h"
#include "rt2dPointDataObject.h"

rt2DPointRenderObject::rt2DPointRenderObject() {
  setObjectType(rtConstants::OT_2DPointBuffer);
  setName("2D Point Renderer");
  setupDataObject();
  setupPipeline();
}


rt2DPointRenderObject::~rt2DPointRenderObject() {
}


//! Take info from the data object. 
void rt2DPointRenderObject::update() {
}

//! Add this object to the given renderer.
bool rt2DPointRenderObject::addToRenderer(vtkRenderer* ren) {
  return true;
}

//! Remove this object from the given renderer.
bool rt2DPointRenderObject::removeFromRenderer(vtkRenderer* ren) {
  return true;
}


//! Create the correct data object.
void rt2DPointRenderObject::setupDataObject() {
  m_dataObj = new rt2DPointDataObject();
}


//! Create the part of the pipeline that is done first. 
void rt2DPointRenderObject::setupPipeline() {
}

//! 2D points do not have locations in 3D space.
bool rt2DPointRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
