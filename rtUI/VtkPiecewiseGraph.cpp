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
}


//! Creates the points to start with
void VtkPiecewiseGraph::createStartUpPoints(QPolygonF & startPoints, QVector<HoverPoints::LockType> & locks) {
    const QRectF & bounds = graphWidget->getSettings().hoverPointsSettings.dataSpace;
    startPoints << QPointF(bounds.left(),  (double)bounds.top()/2)
            << QPointF(bounds.right(), (double)bounds.top()/2);
    locks << HoverPoints::LockToLeft << HoverPoints::LockToRight;
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

//! The function to immitate clamping. If matchVtkPiecewiseFunction is true, will match the profile to piecewiseFunction
void VtkPiecewiseGraph::clamp() {
    HoverPoints & hoverPoints = *graphWidget->getHoverPoints();

    if (!piecewiseFunction->GetClamping()) {
        if (hoverPoints.getPoints().size() >= 2) {
            hoverPoints.removePoint(0);
            hoverPoints.removePoint(graphWidget->getPoints().size() - 1);
        }
        else {
           cerr << "VtkPiecewiseGraph.cpp: error, the graph is not clamped, but has < 2 points." << endl;
        }
    }
    else {
        const QRectF & bounds = graphWidget->getSettings().hoverPointsSettings.dataSpace;
        hoverPoints.addDataPoint(0, QPointF(bounds.left(),  (double)bounds.top()/2));
        hoverPoints.addDataPoint(graphWidget->getPoints().size(), QPointF(bounds.right(), (double)bounds.top()/2));

        hoverPoints.setPointLock(0, HoverPoints::LockToLeft);
        hoverPoints.setPointLock(graphWidget->getPoints().size() - 1, HoverPoints::LockToRight);
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

inline static bool x_less_than(const QPointF &p1, const QPointF &p2) {
    return p1.x() < p2.x();
}

//! Set a new piecewise function and update the GUI. The data space of HoverPoints will change.
bool VtkPiecewiseGraph::setPiecewiseFunction(vtkPiecewiseFunction * const func) {
    if (!func) return false;

    piecewiseFunction->DeepCopy(func);
  
    QPolygonF points;
    double * data = piecewiseFunction->GetDataPointer();
    for (int a = 0; a < piecewiseFunction->GetSize(); a++) {
        points << QPointF(data[2* a], data[2 * a + 1]);
    }
    qSort(points.begin(), points.end(), x_less_than);

    const QRectF & dataSpace = graphWidget->getSettings().hoverPointsSettings.dataSpace;
    QRectF dataSpaceNew;
    dataSpaceNew.setBottom((int)dataSpace.bottom());
    dataSpaceNew.setTop((int)dataSpace.top());

    double range[2];
    piecewiseFunction->GetRange(range);
    //dataSpaceNew.setLeft(((int)points.at(0).x() - ((int)points.at(0).x()) % DATASPACE_MARGIN));
    //dataSpaceNew.setRight(((int)points.at(points.size() - 1).x() - ((int) points.at(points.size() - 1).x()) % DATASPACE_MARGIN + DATASPACE_MARGIN));
    dataSpaceNew.setLeft(range[0]);
    dataSpaceNew.setRight(range[1]);

    HoverPoints & hoverPoints = *graphWidget->getHoverPoints();
    hoverPoints.setDataSpace(dataSpaceNew);
    hoverPoints.setDataPoints(points);

    if (!piecewiseFunction->GetClamping()) {
        hoverPoints.addDataPoint(0, QPointF(dataSpaceNew.left(),  (double)dataSpaceNew.top()/2));
        hoverPoints.addDataPoint(graphWidget->getPoints().size(), QPointF(dataSpaceNew.right(), (double)dataSpaceNew.top()/2));

        hoverPoints.setPointLock(0, HoverPoints::LockToLeft);
        hoverPoints.setPointLock(graphWidget->getPoints().size() - 1, HoverPoints::LockToRight);
    }
    updateVtkPiecewisePoints();
}

//! Checks whethter the graph is clamped whenever the points change
void VtkPiecewiseGraph::checkClamping() {
    const QVector<HoverPoints::LockType> & locks = graphWidget->getLocks();

    if (locks.size() >= 2 && locks.at(0) == HoverPoints::LockToLeft && locks.at(locks.size() - 1) == HoverPoints::LockToRight) {
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
