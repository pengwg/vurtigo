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

#include "rt2DPlotRenderObject.h"
#include "rt2DPlotDataObject.h"

#include <vtkPlot.h>
#include <vtkAxis.h>
#include <vtkContextScene.h>

#include "rtApplication.h"
#include "rtMainWindow.h"


rt2DPlotRenderObject::rt2DPlotRenderObject()
{
    setObjectType("OT_2DPlot");
    setName("2D Plot Renderer");

    m_plot = vtkChartXY::New();

    setupDataObject();
    setupPipeline();
}

rt2DPlotRenderObject::~rt2DPlotRenderObject()
{
    if (m_plot) m_plot->Delete();
    // remove the context view
    rtApplication::instance().getMainWinHandle()->removeContextView(m_view);
}

//! Take info from the data object
void rt2DPlotRenderObject::update()
{

}

//! Create the correct data object
void rt2DPlotRenderObject::setupDataObject()
{
    m_dataObj = new rt2DPlotDataObject();
}

//! Setup the rendering pipeline
void rt2DPlotRenderObject::setupPipeline()
{
    m_canRender3D = false;

    // every plot object creates a new context view for itself
    int index = rtApplication::instance().getMainWinHandle()->addNewPlot();
    m_view = rtApplication::instance().getMainWinHandle()->getContextView(index);

    if (!m_view) return;
    m_view->GetScene()->AddItem(m_plot);
}

//! 2D plots do not have locations in 3D space.
bool rt2DPlotRenderObject::getObjectLocation(double loc[6]) {
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  loc[3] = 0.0;
  loc[4] = 0.0;
  loc[5] = 0.0;

  return false;
}

bool rt2DPlotRenderObject::addToRenderer(vtkRenderer *ren, int window)
{
   return true;
}

bool rt2DPlotRenderObject::removeFromRenderer(vtkRenderer *ren, int window)
{
   return true;
}

vtkPlot *rt2DPlotRenderObject::addNewPlot()
{
    if (!m_plot) return NULL;

    m_plot->AddPlot(vtkChart::LINE);
}

void rt2DPlotRenderObject::createPlot(int colX,int colY)
{
    rt2DPlotDataObject *dObj = static_cast<rt2DPlotDataObject*>(m_dataObj);
    if (colX > dObj->getDataTable()->GetNumberOfColumns()) return;
    if (colY >  dObj->getDataTable()->GetNumberOfColumns()) return;


    vtkPlot *line = addNewPlot();

    line->SetInput(dObj->getDataTable(),colX,colY);
}
