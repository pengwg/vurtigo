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

