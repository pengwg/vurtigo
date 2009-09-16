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
