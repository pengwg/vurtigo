#include "ControlGraphWidget.h"

//! Sets up the widgets
void ControlGraphWidget::init() {
    setWindowTitle(tr("ControlGraphWidget"));
    mainLayout = new QVBoxLayout(this);

    graphWidget = new GraphWidget(this);
    mainLayout->addWidget(graphWidget);

    pointTableWidget = new PointTableWidget(this, &graphWidget->getPoints());
    mainLayout->addWidget(pointTableWidget);

    profileWidget = new ProfileWidget(this, graphWidget->getHoverPoints());
    mainLayout->addWidget(profileWidget);
    connect(profileWidget, SIGNAL(getProfileSignal(ProfileHandler::Profile&)), this, SLOT(getDefaultProfile(ProfileHandler::Profile&)));
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

//! Connect the widgets and load the default profiles
void ControlGraphWidget::startUp() {
    connect(graphWidget, SIGNAL(graphChanged(int)), pointTableWidget, SLOT(updateTable(int)));
    connect(pointTableWidget, SIGNAL(cellUpated(int,bool,QString*const)), graphWidget->getHoverPoints(), SLOT(changeDataPoint(int,bool,QString*const)));
    profileWidget->loadDefaultProfiles();
}

ControlGraphWidget::~ControlGraphWidget() {
    delete graphWidget;
    delete pointTableWidget;
    delete mainLayout;
}

//! Returns the default profile for the entire widget
void ControlGraphWidget::getDefaultProfile(ProfileHandler::Profile & profile) {
    HoverPoints::Profile & hoverPointsProfile = profile.hoverPointsProfile;
    createStartUpPoints(hoverPointsProfile.points, hoverPointsProfile.locks);
    graphWidget->getDefaultDataSpace(hoverPointsProfile.dataSpace);
}

