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
#include "ControlGraphWidget.h"

//! Sets up the widgets
void ControlGraphWidget::init() {
    setWindowTitle(tr("ControlGraphWidget"));
    mainLayout = new QVBoxLayout(this);

    graphWidget = new GraphWidget(this);
    mainLayout->addWidget(graphWidget);

    pointTableWidget = new PointTableWidget(this, &graphWidget->getPoints());
    mainLayout->addWidget(pointTableWidget);
}

ControlGraphWidget::ControlGraphWidget(QWidget *parent) : QWidget(parent) {
}

//! Creates the points to start with
void ControlGraphWidget::createStartUpPoints(QPolygonF & startPoints, QVector<HoverPoints::LockType> & locks) {
    QRectF bounds = graphWidget->getSettings().hoverPointsSettings.dataSpace;
    startPoints << QPointF(bounds.left(),  bounds.bottom() + 1)
            << QPointF(bounds.right() + 1, bounds.top());
    locks << HoverPoints::LockToLeft << HoverPoints::LockToRight;
}

//! Connect the widgets
void ControlGraphWidget::startUp() {
    connect(graphWidget, SIGNAL(graphChanged(int)), pointTableWidget, SLOT(updateTable(int)));
    connect(pointTableWidget, SIGNAL(cellUpated(int,bool,QString*const)), graphWidget->getHoverPoints(), SLOT(changeDataPoint(int,bool,QString*const)));
}

ControlGraphWidget::~ControlGraphWidget() {
    delete graphWidget;
    delete pointTableWidget;
    delete mainLayout;
}

