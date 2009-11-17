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
#ifndef VTK_RANGE_WIDGET_H
#define VTK_RANGE_WIDGET_H

#include <QtGui>
#include "ui_VtkRangeWidget.h"
#include "ui_VtkRangeDialog.h"

#include "HoverPoints.h"

#define DIALOG_WINDOW_TITLE "Select scalar value"
#define SCALAR_MAIN_TEXT "Scalar Value Range: "
#define SCALAR_TO_TEXT " to "

//! Dialog to define the range of the scalar value
class VtkRangeDialog : public QDialog, protected Ui::VtkRangeDialog {
    Q_OBJECT

    HoverPoints * hoverPoints;
public:
    VtkRangeDialog(QWidget * parent = NULL, HoverPoints * hoverPoints = NULL);
    HoverPoints & getHoverPoints() { return *hoverPoints; }
public slots:
    void refresh();
private slots:
    void applyChanges();
signals:
    void applyChangesSignal(const QRectF & dataSpace);
};

//! Widget that displays and allows modification of the scalar value
class VtkRangeWidget : public QWidget , protected Ui::VtkRangeWidget {
    Q_OBJECT

    VtkRangeDialog *rangeDialog;
public:
    VtkRangeWidget(QWidget * parent = NULL, HoverPoints * hoverPoints = NULL);
    ~VtkRangeWidget();
private slots:
    void changeDialog();
    void labelUpdate(const QRectF & dataSpace);
};

#endif // GRAPH_WIDGET_CONTROLS_H
