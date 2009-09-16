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
    void getDefaultProfile(ProfileHandler::Profile & profile);
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
