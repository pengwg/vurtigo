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
