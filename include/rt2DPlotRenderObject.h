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
