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
#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <QtGui/QWidget>

#include "HoverPoints.h"

class GraphWidgetSet: public QWidget {
};

//! Parent widget of HoverPoints, such that HoverPoints can paint on it as a rectangle
class GraphWidget : public QWidget {
    Q_OBJECT

public:
    struct Settings {
        //! Background color of rectangle
        QColor background;
        //! Outline color of rectangle
        QColor outline;
        HoverPoints::Settings hoverPointsSettings;
    };

    GraphWidget(QWidget * parent = NULL, const Settings * const settings = NULL);
    ~GraphWidget();
    Settings & getSettings() { return settings; }
    const QPolygonF & getPoints() { return hoverPoints->getPoints(); }
    const QVector<HoverPoints::LockType> & getLocks() { return hoverPoints->getLocks(); }

    virtual QSize sizeHint() const { return QSize(300, 100); }

    void paintEvent(QPaintEvent * e);
    virtual void setDefaultSettings();
    virtual void getDefaultDataSpace(QRectF & dataSpace);
signals:
    void graphChanged(int index);

protected:
    HoverPoints * hoverPoints;
    Settings settings;
public:
    HoverPoints * const getHoverPoints() { return hoverPoints; }
};

#endif // GRAPH_WIDGET_H
