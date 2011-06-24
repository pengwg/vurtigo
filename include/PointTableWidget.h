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
#ifndef POINT_TABLE_WIDGET_H
#define POINT_TABLE_WIDGET_H

#include <QTableWidget>

#define DEFAULT_COLUMN_WIDTH 100

//! Table widget that tracks GraphWidget's values and changes them
class PointTableWidget : public QTableWidget {
    Q_OBJECT
public:
    struct Settings {
        //! Index of x column
        int xColumn;
        //! Index of y column
        int yColumn;

        //! Headers of columns
        QStringList colHeader;

        //! True if the table will select the auto-select the row corresponding to the point being modified
        bool autoSelectRow;
        QVector<unsigned int> columnWidths;
    };

    PointTableWidget(QWidget * parent = NULL, const QPolygonF * const points = NULL, const Settings * const settings = NULL);
    ~PointTableWidget();
public slots:
    void updateTable(int row = -1);
signals:
    void cellUpated(int index, bool isX, QString * const value);
private:
    QPoint editingCell;
    void setColumnWidths();
    void setDefaultSettings();
private slots:
    void cellEditingSlot(int row, int column);
    void updateCell(int row, int column);
protected:
    Settings settings;
    const QPolygonF & points;

    virtual void createNewRow(int row);
    virtual void changeRow(int row);
};

#endif // POINT_TABLE_WIDGET_H
