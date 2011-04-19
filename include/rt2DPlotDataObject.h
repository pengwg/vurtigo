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
#ifndef RT2DPLOTDATAOBJECT_H
#define RT2DPLOTDATAOBJECT_H

#include "rtDataObject.h"
#include <vtkTable.h>
#include <vtkDataArray.h>

//! Object that represents a 2D XY plot
/*!
  A data representation of a 2D Plot
  */

class rt2DPlotDataObject : public rtDataObject
{
Q_OBJECT

public:

    rt2DPlotDataObject();
    ~rt2DPlotDataObject();
    //! Get the data table
    vtkTable *getDataTable() {return m_dataTable;}
    //! Add a new column of data to the table
    void addDataArray(vtkDataArray *data) { m_dataTable->AddColumn(data); Modified(); }
    //! Copy an existing table of data
    void setTableData(vtkTable *newTable) { m_dataTable->DeepCopy(newTable); Modified(); }
    //! Set the table value at a given row/col
    void setTableItem(int row, int col, double val) { m_dataTable->SetValue(row,col,val); Modified(); }

    void update();

protected:
    //Functions
    void setupGUI();
    void cleanupGUI();

    vtkTable *m_dataTable;


};

#endif // RT2DPLOTDATAOBJECT_H
