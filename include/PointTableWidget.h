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
