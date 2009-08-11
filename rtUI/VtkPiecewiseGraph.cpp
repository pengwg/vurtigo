#include "VtkPiecewiseGraph.h"
#include "GraphWidget.h"
#include "PointTableWidget.h"

#include <iostream>
using namespace std;

ClampingWidget::ClampingWidget(QWidget * parent) : QWidget(parent) {
    setupUi(this);
    connect(mainButton, SIGNAL(clicked()), (VtkPiecewiseGraph *) parent, SLOT(clamp()));
}

ClampingWidget::~ClampingWidget() {

}

void ClampingWidget::setMainLabelText(const QString & text) {
    mainLabel->setText(text);
}

static inline void setTableSettings(PointTableWidget::Settings & settings) {
    QStringList colHeader;
    colHeader << ("Scalar Value") << ("Opacity");
    settings.colHeader = colHeader;

    settings.xColumn = 0;
    settings.yColumn = 1;

    settings.autoSelectRow = true;
}

static inline void setGraphSettings(GraphWidget::Settings & settings) {
    settings.background = Qt::white;
    settings.outline = Qt::black;

    HoverPoints::Settings & hoverPointsSettings = settings.hoverPointsSettings;

    hoverPointsSettings.sortType = HoverPoints::SortByX;
    hoverPointsSettings.dataSpace = QRect(0, SPACE_HEIGHT, DEFAULT_SPACE_WIDTH, -SPACE_HEIGHT);

    hoverPointsSettings.pointShape = HoverPoints::Cirle;
    hoverPointsSettings.pointSize = QSize(11, 11);
    hoverPointsSettings.pointOutline = QPen(Qt::black, 1);
    hoverPointsSettings.pointFill = QBrush(Qt::white);

    hoverPointsSettings.connectionType = HoverPoints::LineConnection;
    hoverPointsSettings.connectionLine = QPen(Qt::black, 1);

    hoverPointsSettings.followingEnds = true;
}

void VtkPiecewiseGraph::init() {
    setWindowTitle(tr("VtkPiecewiseGraph"));
    mainLayout = new QVBoxLayout(this);

    GraphWidget::Settings graphSettings;
    setGraphSettings(graphSettings);
    graphWidget = new GraphWidget(this, &graphSettings);
    mainLayout->addWidget(graphWidget);

    PointTableWidget::Settings tableSettings;
    setTableSettings(tableSettings);
    pointTableWidget = new PointTableWidget(this, &graphWidget->getPoints(), &tableSettings);
    mainLayout->addWidget(pointTableWidget);

    clampingWidget = new ClampingWidget(this);
    mainLayout->addWidget(clampingWidget);

    rangeWidget = new VtkRangeWidget(this, graphWidget->getHoverPoints());
    mainLayout->addWidget(rangeWidget);

    profileWidget = new ProfileWidget(this, graphWidget->getHoverPoints());
    mainLayout->addWidget(profileWidget);
    connect(profileWidget, SIGNAL(getProfileSignal(ProfileHandler::Profile&)), this, SLOT(getDefaultProfile(ProfileHandler::Profile&)));
}


//! Creates the points to start with
void VtkPiecewiseGraph::createStartUpPoints(QPolygonF & startPoints, QVector<HoverPoints::LockType> & locks) {
    QRectF bounds = graphWidget->getSettings().hoverPointsSettings.dataSpace;
    startPoints << QPointF(bounds.left(),  (double)bounds.top()/2)
            << QPointF(bounds.right(), (double)bounds.top()/2);
    locks << HoverPoints::LockToLeft << HoverPoints::LockToRight;
}


//! Returns the default new profile
void VtkPiecewiseGraph::getDefaultProfile(ProfileHandler::Profile & profile) {
    HoverPoints::Profile & hoverPointsProfile = profile.hoverPointsProfile;
    createStartUpPoints(hoverPointsProfile.points, hoverPointsProfile.locks);
    hoverPointsProfile.dataSpace = QRect(0, SPACE_HEIGHT, DEFAULT_SPACE_WIDTH, -SPACE_HEIGHT);
}

VtkPiecewiseGraph::VtkPiecewiseGraph(QWidget *parent) : ControlGraphWidget(parent) {
    init();
    startUp();

    QPolygonF startPoints;
    QVector<HoverPoints::LockType> locks;

    piecewiseFunction = vtkPiecewiseFunction::New();
    clampingWidget->setMainLabelText(QString(CLAMPING_LABEL_TEXT) + QString("On"));
    piecewiseFunction->ClampingOn();
    updateVtkPiecewisePoints();
    connect(graphWidget, SIGNAL(graphChanged(int)), this, SLOT(updateVtkPiecewisePoints()));
}

VtkPiecewiseGraph::~VtkPiecewiseGraph() {
}

//! The function to immitate clamping
void VtkPiecewiseGraph::clamp() {
    HoverPoints & hoverPoints = *graphWidget->getHoverPoints();

    if (piecewiseFunction->GetClamping()) {
        QRectF bounds = graphWidget->getSettings().hoverPointsSettings.dataSpace;
        hoverPoints.addDataPoint(0, QPointF(bounds.left(),  (double)bounds.top()/2));
        hoverPoints.addDataPoint(graphWidget->getPoints().size() - 1, QPointF(bounds.right(), (double)bounds.top()/2));

        hoverPoints.setPointLock(0, HoverPoints::LockToLeft);
        hoverPoints.setPointLock(graphWidget->getPoints().size() - 1, HoverPoints::LockToRight);
    }
    else {
        if (hoverPoints.getPoints().size() > 2) {
            hoverPoints.removePoint(0);
            hoverPoints.removePoint(graphWidget->getPoints().size() - 1);
        }
    }
    updateVtkPiecewisePoints();
}

//! Updates the piecewise points
void VtkPiecewiseGraph::updateVtkPiecewisePoints() {
    int start;
    int end;
    const QPolygonF & points = graphWidget->getPoints();

    checkClamping();

    if (piecewiseFunction->GetClamping()) {
        start = 0;
        end = points.size();
    }
    else {
        start = 1;
        end = points.size() - 1;
    }

    piecewiseFunction->RemoveAllPoints();
    for (int a = start; a < end; a++) {
        piecewiseFunction->AddPoint(points.at(a).x(), points.at(a).y());
    }
    piecewiseFunction->Update();
    emit functionUpdated();
    //cout << *piecewiseFunction << endl;
}

//! Returns a pointer to the internal piecwise function
vtkPiecewiseFunction * VtkPiecewiseGraph::getPiecewiseFunction() {
    return piecewiseFunction;
}

//! Set a new piecewise function and update the GUI.
/*!
  @todo Update the GUI here.
  */
bool VtkPiecewiseGraph::setPiecewiseFunction(vtkPiecewiseFunction * func) {
  if (!func) return false;

  piecewiseFunction = func;
  HoverPoints::Profile newProfile;

}

//! Checks whethter the graph is clamped whenever the points change
void VtkPiecewiseGraph::checkClamping() {
    const QVector<HoverPoints::LockType> & locks = graphWidget->getLocks();

    if (locks.at(0) == HoverPoints::LockToLeft && locks.at(locks.size() - 1) == HoverPoints::LockToRight) {
        if (piecewiseFunction->GetClamping()) {
            clampingWidget->setMainLabelText(QString(CLAMPING_LABEL_TEXT) + QString("Off"));
            piecewiseFunction->ClampingOff();
        }
    }
    else {
        if (!piecewiseFunction->GetClamping()) {
            clampingWidget->setMainLabelText(QString(CLAMPING_LABEL_TEXT) + QString("On"));
            piecewiseFunction->ClampingOn();
        }
    }
    piecewiseFunction->Update();
    emit functionUpdated();
}
