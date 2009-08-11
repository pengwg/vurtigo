#ifndef CONTROL_GRAPH_WIDGET_H
#define CONTROL_GRAPH_WIDGET_H

#include <QtGui/QWidget>

#include "GraphWidget.h"
#include "PointTableWidget.h"
#include "ProfileWidget.h"

//! Base class for the main widget that holds everything
class ControlGraphWidget : public QWidget {
    Q_OBJECT

    virtual void createStartUpPoints(QPolygonF & startPoints, QVector<HoverPoints::LockType> & locks);
protected:
    ControlGraphWidget(QWidget *parent = NULL);
    ~ControlGraphWidget();

    QLayout * mainLayout;
    //! Table that represents the points
    PointTableWidget * pointTableWidget;
    //! The widget with the points
    GraphWidget * graphWidget;
    //! Widget that saves the profile for the points
    ProfileWidget * profileWidget;

    virtual void init();
    virtual void startUp();
public slots:
    virtual void getDefaultProfile(ProfileHandler::Profile & profile);
};

#endif // CONTROL_GRAPH_WIDGET_H
