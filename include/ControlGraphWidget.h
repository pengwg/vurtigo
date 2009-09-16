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
