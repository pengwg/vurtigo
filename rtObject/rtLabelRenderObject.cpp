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
#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType(rtConstants::OT_TextLabel);
  setName("Simple Label Renderer");
  setupDataObject();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
  if (m_dataObj) delete m_dataObj;
  if (m_textActor2D) m_textActor2D->Delete();
}


//! Take the info from the DataObject
void rtLabelRenderObject::update() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());

  if (rtApplication::instance().getMainWinHandle() && m_textActor2D->GetNumberOfConsumers() > 0) {
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

//! Add this object to the given renderer.
bool rtLabelRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(true);
  if (ren->HasViewProp(m_textActor2D)) return false;
  ren->AddViewProp(m_textActor2D);
  return true;
}

//! Remove this object from the given renderer.
bool rtLabelRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(false);
  if (!ren->HasViewProp(m_textActor2D)) return false;
  ren->RemoveViewProp(m_textActor2D);
  return true;
}

void rtLabelRenderObject::setupDataObject() {
  m_dataObj = new rtLabelDataObject();
  
}

void rtLabelRenderObject::setupPipeline() {
  rtLabelDataObject *dObj = dynamic_cast<rtLabelDataObject*>(m_dataObj);

  m_textActor2D = vtkTextActor::New();
  m_textActor2D->SetInput(dObj->getText().toStdString().c_str());
  m_textActor2D->SetTextProperty(dObj->getTextProperty());
  m_textActor2D->SetTextScaleModeToNone();

  m_canRender3D = true;

  m_pipe2D.insert("Label", m_textActor2D);
}

//! This object has no valid 3D location.
bool rtLabelRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}
