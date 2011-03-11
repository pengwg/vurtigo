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
#include "rtImageBufferRenderObject.h"
#include "rtImageBufferDataObject.h"

rtImageBufferRenderObject::rtImageBufferRenderObject() {
  setObjectType(rtConstants::OT_ImageBuffer);
  setName("Image Buffer Renderer");
  setupDataObject();
  setupPipeline();
}


rtImageBufferRenderObject::~rtImageBufferRenderObject() {
}


//! Take info from the data object. 
void rtImageBufferRenderObject::update() {
}

//! Add this object to the given renderer.
bool rtImageBufferRenderObject::addToRenderer(vtkRenderer* ren,int window) {

}

//! Remove this object from the given renderer.
bool rtImageBufferRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {

}

//! Create the correct data object.
void rtImageBufferRenderObject::setupDataObject() {
  m_dataObj = new rtImageBufferDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtImageBufferRenderObject::setupPipeline() {
}

//! When this object is implemented it will have a valid location. For now it does not.
bool rtImageBufferRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
