#include "GraphWidget.h"

void GraphWidget::getDefaultDataSpace(QRectF & dataSpace) {
    dataSpace = QRectF(0, sizeHint().height(), sizeHint().width(), -sizeHint().height());
}

void GraphWidget::setDefaultSettings() {
    settings.background = Qt::white;
    settings.outline = Qt::black;

    HoverPoints::Settings & hoverPointsSettings = settings.hoverPointsSettings;

    hoverPointsSettings.sortType = HoverPoints::SortByX;
    getDefaultDataSpace(hoverPointsSettings.dataSpace);

    hoverPointsSettings.pointShape = HoverPoints::Cirle;
    hoverPointsSettings.pointSize = QSize(11, 11);
    hoverPointsSettings.pointOutline = QPen(Qt::black, 1);
    hoverPointsSettings.pointFill = QBrush(Qt::white);

    hoverPointsSettings.connectionType = HoverPoints::LineConnection;
    hoverPointsSettings.connectionLine = QPen(Qt::black, 1);

    hoverPointsSettings.followingEnds = false;
}

GraphWidget::GraphWidget(QWidget * parent, const Settings * const settings) : QWidget(parent) {
    //Optimize
    setAttribute(Qt::WA_OpaquePaintEvent);

    if (settings == NULL)
        setDefaultSettings();
    else
        this->settings = *settings;

    hoverPoints = new HoverPoints(this, this->settings.hoverPointsSettings);

    //Hozontal can change, veritcal is fixed
    resize(sizeHint());
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(hoverPoints, SIGNAL(pointsChanged(int)), this, SIGNAL(graphChanged(int)));
}

GraphWidget::~GraphWidget() {
    delete hoverPoints;
}

//! Paints the widget for the next frame, by painting over everything and then letting HoverPoints paint
void GraphWidget::paintEvent(QPaintEvent *) {
    QPainter  p(this);
    p.fillRect(rect(), QBrush(settings.background));

    p.setPen(settings.outline);
    p.drawRect(0, 0, width() - 1, height() - 1);
}
