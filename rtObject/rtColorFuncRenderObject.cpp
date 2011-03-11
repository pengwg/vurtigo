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
#include "rtColorFuncRenderObject.h"
#include "rtColorFuncDataObject.h"

rtColorFuncRenderObject::rtColorFuncRenderObject() {
  setObjectType(rtConstants::OT_vtkColorTransferFunction);
  setName("Color Transfer Func Renderer");
  setupDataObject();
  setupPipeline();
}


rtColorFuncRenderObject::~rtColorFuncRenderObject() {
}


//! Take info from the data object. 
void rtColorFuncRenderObject::update() {
}

//! Add this object to the given renderer.
bool rtColorFuncRenderObject::addToRenderer(vtkRenderer* ren, int window) {

}

//! Remove this object from the given renderer.
bool rtColorFuncRenderObject::removeFromRenderer(vtkRenderer* ren, int window) {

}

//! Create the correct data object.
void rtColorFuncRenderObject::setupDataObject() {
  m_dataObj = new rtColorFuncDataObject();
}


//! Create the part of the pipeline that is done first. 
void rtColorFuncRenderObject::setupPipeline() {
}

//! This object does not have a valid 3D location.
bool rtColorFuncRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
