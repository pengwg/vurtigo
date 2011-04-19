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
