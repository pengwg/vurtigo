/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "rtLabelRenderObject.h"
#include "rtLabelDataObject.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

rtLabelRenderObject::rtLabelRenderObject() {
  setObjectType("OT_TextLabel");
  setName("Simple Label Renderer");
  setupDataObject();
  setupPipeline();
}

rtLabelRenderObject::~rtLabelRenderObject() {
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
  setVisible3D(window,true);
  if (ren->HasViewProp(m_textActor2D)) return false;
  ren->AddViewProp(m_textActor2D);
  return true;
}

//! Remove this object from the given renderer.
bool rtLabelRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,false);
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
