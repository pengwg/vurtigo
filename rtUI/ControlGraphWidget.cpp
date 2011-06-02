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

