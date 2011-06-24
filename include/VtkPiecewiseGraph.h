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
#ifndef VTK_PIECEWISE_GRAPH_H
#define VTK_PIECEWISE_GRAPH_H
#include "ControlGraphWidget.h"
#include "ui_VtkClampingWidget.h"
#include "VtkRangeWidget.h"
#include <vtkPiecewiseFunction.h>

#define SPACE_HEIGHT 1
#define DEFAULT_SPACE_WIDTH 300
#define CLAMPING_LABEL_TEXT "Clamping: "
#define DATASPACE_MARGIN 50

//! Widget to clamp the vtkPiecewiseFunction
class ClampingWidget : public QWidget , protected Ui::ClampingWidget {
    Q_OBJECT
public:
    ClampingWidget(QWidget * parent = NULL);
    ~ClampingWidget();
public slots:
    void setMainLabelText(const QString & text);
};

//! Dervived class that contains the rest of the widgets
class VtkPiecewiseGraph : public ControlGraphWidget {
    Q_OBJECT

    vtkPiecewiseFunction * piecewiseFunction;
public:
    VtkPiecewiseGraph(QWidget *parent = NULL);
    ~VtkPiecewiseGraph();

    vtkPiecewiseFunction * getPiecewiseFunction();
    bool setPiecewiseFunction(vtkPiecewiseFunction * const func);

public slots:
    void clamp();
    void updateVtkPiecewisePoints();
signals:
  void functionUpdated();

protected:
    //! Widget to clamp the vtkPiecewiseFunction
    ClampingWidget * clampingWidget;
    //! Widget to change the x range of the graph
    VtkRangeWidget * rangeWidget;

    virtual void init();
    virtual void createStartUpPoints(QPolygonF & startPoints, QVector<HoverPoints::LockType> & locks);
private:
    void checkClamping();
};


#endif // VTK_PIECEWISE_GRAPH_H
