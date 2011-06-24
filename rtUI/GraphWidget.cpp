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
