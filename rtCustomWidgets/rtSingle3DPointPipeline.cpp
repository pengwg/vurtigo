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
#include "rtSingle3DPointPipeline.h"
#include "rtApplication.h"
#include "rtMainWindow.h"

rtSingle3DPointPipeline::rtSingle3DPointPipeline() {
  m_actor = vtkActor::New();

  m_labelMapper = vtkPolyDataMapper::New();
  m_labelText = vtkVectorText::New();
  m_labelMapper->SetInputConnection(m_labelText->GetOutputPort());
  for (int ix1=0; ix1 < rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
  {
      m_labelActor.append(vtkFollower::New());
      m_labelActor.last()->SetMapper(m_labelMapper);
  }
}

rtSingle3DPointPipeline::~rtSingle3DPointPipeline() {

  m_actor->Delete();
  m_labelMapper->Delete();
  m_labelText->Delete();
  for (int ix1=0; ix1<m_labelActor.size(); ix1++)
      m_labelActor[ix1]->Delete();
}

void rtSingle3DPointPipeline::setPosition(double x, double y, double z) {
  m_actor->SetPosition(x,y,z);
}

void rtSingle3DPointPipeline::setPosition(double coords[3]) {
  m_actor->SetPosition(coords);
}

void rtSingle3DPointPipeline::setLabelPosition(int i, double x, double y, double z) {
    if (i < m_labelActor.size())
        m_labelActor[i]->SetPosition(x,y,z);
}

void rtSingle3DPointPipeline::setLabelPosition(int i, double coords[3]) {
    if (i < m_labelActor.size())
        m_labelActor[i]->SetPosition(coords);
}

void rtSingle3DPointPipeline::setRadius(double r) {
  if (r<=0.0) return;
  m_actor->SetScale(r);
}

void rtSingle3DPointPipeline::setLabelScale(int i, double s)
{
    if (i < m_labelActor.size())
        m_labelActor[i]->SetScale(s);
}

void rtSingle3DPointPipeline::setLabelCamera(int i, vtkCamera *cam)
{
    if (i < m_labelActor.size())
        m_labelActor[i]->SetCamera(cam);
}

void rtSingle3DPointPipeline::setLabelText(const char *text)
{
        m_labelText->SetText(text);
}

void rtSingle3DPointPipeline::setProperty(vtkProperty* prop) {
  if (!prop) return;
  m_actor->SetProperty(prop);
}

void rtSingle3DPointPipeline::setNumRenWin(int num)
{
    // add any new actors if needed
    int ix1;
    for (ix1=0; ix1<(num - m_labelActor.size()); ix1++)
    {
        m_labelActor.append(vtkFollower::New());
        m_labelActor.last()->SetMapper(m_labelMapper);
    }
}
