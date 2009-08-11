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
