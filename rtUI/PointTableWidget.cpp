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
#include "PointTableWidget.h"

void PointTableWidget::setDefaultSettings() {
    QStringList colHeader;
    colHeader << ("x") << ("y");
    settings.colHeader = colHeader;

    settings.xColumn = 0;
    settings.yColumn = 1;

    settings.autoSelectRow = true;
}

void PointTableWidget::setColumnWidths() {
    if (this->settings.columnWidths.size() <= 1) {
        unsigned int columnWidth;
        if (this->settings.columnWidths.size() == 0)
            columnWidth = DEFAULT_COLUMN_WIDTH;
        else
            columnWidth = this->settings.columnWidths.at(0);

        for (int a = 0; a < columnCount(); a++) {
            setColumnWidth(a, columnWidth);
        }
    }
    else {
        for (int a = 0; a < columnCount(); a++) {
            setColumnWidth(a, this->settings.columnWidths.at(a));
        }
    }
}

PointTableWidget::PointTableWidget(QWidget * parent, const QPolygonF * const points, const Settings * const settings) : QTableWidget(parent) , points(*points) {
    if (settings == NULL)
        setDefaultSettings();
    else
        this->settings = *settings;

    setColumnCount(this->settings.colHeader.size());
    setHorizontalHeaderLabels(this->settings.colHeader);

    setColumnWidths();

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    if (this->settings.autoSelectRow)
        setSelectionBehavior(QAbstractItemView::SelectRows);

    editingCell = QPoint(-1, -1);
    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellEditingSlot(int,int)));
    connect(this, SIGNAL(cellChanged(int,int)), this, SLOT(updateCell(int,int)));
}

PointTableWidget::~PointTableWidget() {
}

//! Creates a new row that matches points.at(row)
void PointTableWidget::createNewRow(int row) {
    setRowCount(rowCount() + 1);

    QTableWidgetItem * newItem = new QTableWidgetItem(tr("%1").arg(points.at(row).x()));
    setItem(row, settings.xColumn, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(points.at(row).y()));
    setItem(row, settings.yColumn, newItem);
}

//! Updates the row to match points.at(row)
void PointTableWidget::changeRow(int row) {
    QTableWidgetItem * item = this->item(row, settings.xColumn);
    item->setText(tr("%1").arg(points.at(row).x()));

    item = this->item(row, settings.yColumn);
    item->setText(tr("%1").arg(points.at(row).y()));
}

//! Updates the entire row
void PointTableWidget::updateTable(int row) {
        //Select the row representing the point
        if (settings.autoSelectRow && row != -1) {
            setCurrentCell(row, 1);
        }

        //Remove any unneeded rows
        while (points.size() < rowCount())
            removeRow(rowCount() - 1);

        for (int a = 0; a < points.size(); a++) {
            //Create and change rows as nessecary
            if (a >= rowCount())
                createNewRow(a);
            else
                changeRow(a);
        }
}

//! Method to keep track of the cell being edited by the user, such that updateCell() does not cause recursion
void PointTableWidget::cellEditingSlot(int row, int column) {
    editingCell = QPoint(row, column);
}

//! Updates the points from user input in the table
void PointTableWidget::updateCell(int row, int column) {
    //User edited the cell
    if (editingCell == QPoint(row, column)) {
        editingCell = QPoint(-1, -1);
        if (column == settings.xColumn || column == settings.yColumn) {
            bool isX;
            if (column == settings.xColumn)
                isX = true;
            else
                isX = false;
            QString cellText = item(row, column)->text();
            emit cellUpated(row, isX, &cellText);
        }
    }
    else {
        editingCell = QPoint(-1, -1);
    }
}
