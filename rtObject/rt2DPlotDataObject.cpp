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
#include "rt2DPlotDataObject.h"

//! Constructor
rt2DPlotDataObject::rt2DPlotDataObject()
{
    setObjectType("OT_2DPlot");

    m_dataTable = vtkTable::New();

    setupGUI();
}

//! Destructor
rt2DPlotDataObject::~rt2DPlotDataObject()
{
    if (m_dataTable) m_dataTable->Delete();
    cleanupGUI();
}

//! Send the info to the GUI
void rt2DPlotDataObject::update()
{
}

//! Set the GUI widgets
void rt2DPlotDataObject::setupGUI()
{
}

//! Clean the GUI widgets
void rt2DPlotDataObject::cleanupGUI()
{
}
