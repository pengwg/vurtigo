#ifndef VTK_PIECEWISE_GRAPH_H
#define VTK_PIECEWISE_GRAPH_H
#include "ControlGraphWidget.h"
#include "ui_VtkClampingWidget.h"
#include "VtkRangeWidget.h"
#include <vtkPiecewiseFunction.h>

#define SPACE_HEIGHT 1
#define DEFAULT_SPACE_WIDTH 300
#define CLAMPING_LABEL_TEXT "Clamping: "

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
    bool setPiecewiseFunction(vtkPiecewiseFunction * func);

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
