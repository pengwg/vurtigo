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
#ifndef RT2DPLOTRENDEROBJECT_H
#define RT2DPLOTRENDEROBJECT_H

#include "rtRenderObject.h"

#include <vtkChartXY.h>
#include <vtkChart.h>
#include <vtkContextView.h>
#include <vtkPlot.h>

//! The render object for 2DPlots

class rt2DPlotRenderObject : public rtRenderObject
{

public:

    rt2DPlotRenderObject();
    ~rt2DPlotRenderObject();

    virtual bool addToRenderer(vtkRenderer *ren, int window);
    virtual bool removeFromRenderer(vtkRenderer *ren, int window);

    virtual bool getObjectLocation(double loc[6]);
    //! Create a new XY plot from 2 given table column indices
    void createPlot(int colX,int colY);
    //! Get the XY plot at a given index
    vtkPlot *getPlotAtIndex(int index) { if (m_plot) return m_plot->GetPlot(index); }
    //! The XY chart
    vtkChartXY *getChart() { return m_plot; }

protected:
    void setupDataObject();
    void setupPipeline();

    void update();

    //! Add a new XY plot to the chart
    vtkPlot *addNewPlot();

    //! The VTK chart context item
    vtkChartXY *m_plot;
    //! The context view this plot belongs to
    vtkContextView *m_view;
};

#endif // RT2DPLOTRENDEROBJECT_H
