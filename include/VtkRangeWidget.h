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
