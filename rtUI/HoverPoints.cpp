#include "HoverPoints.h"

HoverPoints::HoverPoints(QWidget * parent, Settings & settings) : QObject(parent), settings(settings) {
    this->parent = parent;
    currentIndex = -1;
    parent->installEventFilter(this);

    connect(this, SIGNAL(pointsChanged(int)), parent, SLOT(update()));
}

//! Given an index of a point, it returns the index of the closest unlocked point
int HoverPoints::findUnboundedPoint(int index) {
    LockType lock = (LockType)locks.at(index);
    if ((lock == LockToLeft || lock == LockToTop) && index + 1 < points.size())
        return points.at(index) == points.at(index + 1) ? index + 1 : index;
    else if ((lock == LockToRight || lock == LockToBottom) && index - 1 >= 0)
        return points.at(index) == points.at(index - 1) ? index - 1 : index;

    return index;
}

//! Given the amount of visible space, space point, and lock, it returns the data space point
QPointF HoverPoints::bound_point(const QPointF &spacePoint, const QRectF &bounds, int lock) {
    QPointF p = spacePoint;

    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = bounds.top();
    qreal bottom = bounds.bottom();
    
    if (p.x() < left || (lock & HoverPoints::LockToLeft)) p.setX(left);
    else if (p.x() > right || (lock & HoverPoints::LockToRight)) p.setX(right);

    if (p.y() < top || (lock & HoverPoints::LockToTop)) p.setY(top);
    else if (p.y() > bottom || (lock & HoverPoints::LockToBottom)) p.setY(bottom);

    p = getDataPoint(p);
    return p;
}

//! Given space points, reset, then store the points within this instance.
void HoverPoints::setSpacePoints(const QPolygonF & points) {
    this->points.clear();
    for (int a = 0; a < points.size(); a++) {
        this->points << bound_point(points.at(a), parent->rect(), 0);
        emit pointsChanged(a);
    }

    locks.clear();
    if (this->points.size() > 0) {
        locks.resize(this->points.size());
        locks.fill(HoverPoints::NoLock);
    }
    parent->update();
}

//! Given the data points, reset, then store the points within this instance
void HoverPoints::setDataPoints(const QPolygonF & points) {
    QPolygonF spacePoints;
    for (int a = 0; a < points.size(); a++)
        spacePoints << getSpacePoint(points.at(a));
    setSpacePoints(spacePoints);
}

//! Attempts to move the point at index, index, to point. calls firePointChange() if emitUpdate = true.
void HoverPoints::movePoint(int index, const QPointF &point, bool emitUpdate) {
    points[index] = bound_point(point, parent->rect(), locks.at(index));
    if (emitUpdate)
        firePointChange();
}

//! Main method that handles events on the parent widget
bool HoverPoints::eventFilter(QObject *object, QEvent *event) {
    if (object == parent) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            //save the postion
            QMouseEvent * mouseEvent = (QMouseEvent *) event;

            QPointF clickPos = mouseEvent->pos();
            int index = -1;

            //Draw a path with the existing points and see if the click postion is within it
            for (int a = 0; a < points.size(); a++) {
                QPainterPath pPath;
                if (settings.pointShape == Cirle)
                    pPath.addEllipse(pointBoundingRect(a));
                else
                    pPath.addRect(pointBoundingRect(a));

                //if the click position is in the path, find the index of the unlocked point
                if (pPath.contains(clickPos)) {
                    index = findUnboundedPoint(a);
                    break;
                }
            }

            if (mouseEvent->button() == Qt::LeftButton) {
                //if the click position doesn't match any existing points
                if (index == -1) {
                    int pos = 0;

                    //Find the index for the new point
                    if (settings.sortType == SortByX) {
                        for (int a = 0; a < points.size(); a++) {
                            if (spacePointAt(a).x() > clickPos.x()) {
                                pos = a;
                                break;
                            }
                        }
                    }
                    else if (settings.sortType == SortByY) {
                        for (int a = 0; a < points.size(); a++) {
                            if (spacePointAt(a).y() > clickPos.y()) {
                                pos = a;
                                break;
                            }
                        }
                    }
                    //add point
                    points.insert(pos, getDataPoint(clickPos));
                    locks.insert(pos, NoLock);
                    currentIndex = pos;
                    //sort points and notify change
                    firePointChange();
                }
                else {
                    //set the index of the current modified point to the closest unlocked point
                    currentIndex = findUnboundedPoint(index);
                }
                return true;
            }
            else if (mouseEvent->button() == Qt::RightButton) {
                //if the click position is on a point, remove that point
                if (index >= 0) {
                    if (locks[index] == 0) {
                        locks.remove(index);
                        points.remove(index);
                    }
                    //sort points and notify change
                    firePointChange();
                    return true;
                }
            }
        }
        break;

        //Mouse button is release for moving the point, set the index of the current modified point to invalid
        case QEvent::MouseButtonRelease: {
            currentIndex = -1;
            break;
        }
        case QEvent::MouseMove: {
            //if the index of the current modified point is valid, move the point to the click position
            if (currentIndex >= 0)
                movePoint(currentIndex, ((QMouseEvent *)event)->pos());
            break;
        }
        case QEvent::Paint: {
            //paint the points
            QWidget * tempParent = parent;
            parent = NULL;
            QApplication::sendEvent(object, event);
            parent = tempParent;
            paintPoints();
            return true;
        }
        default:
            break;
        }
    }
    return false;
}

//! Returns the RectF of space the points take
inline QRectF HoverPoints::pointBoundingRect(int i) const {
    QPointF p = spacePointAt(i);
    qreal w = settings.pointSize.width();
    qreal h = settings.pointSize.height();
    qreal x = p.x() - w / 2;
    qreal y = p.y() - h / 2;
    return QRectF(x, y, w, h);
}

//! Paint the points on the parent
void HoverPoints::paintPoints() {
    QPainter p(parent);
    p.setRenderHint(QPainter::Antialiasing);

    //Draw connecting lines
    if (settings.connectionLine.style() != Qt::NoPen && settings.connectionType != NoConnection) {
        p.setPen(settings.connectionLine);

        if (settings.connectionType == CurveConnection) {
            QPainterPath path;
            path.moveTo(spacePointAt(0));
            for (int a = 1; a < points.size(); a++) {
                QPointF point1 = spacePointAt(a -1);
                QPointF point2 = spacePointAt(a);
                qreal dist = point2.x() - point1.x();

                path.cubicTo(point1.x() + dist / 2, point1.y(),
                             point1.x() + dist / 2, point2.y(),
                             point2.x(), point2.y());
            }
            p.drawPath(path);
        }
        else {
            for (int a = 1; a < points.size(); a++) {
                p.drawLine(getSpacePoint(points[a-1]), getSpacePoint(points[a]));
            }
        }
    }

    //Draw the points
    p.setPen(settings.pointOutline);
    p.setBrush(settings.pointFill);

    for (int a = 0; a < points.size(); a++) {
        QRectF bounds = pointBoundingRect(a);
        if (settings.pointShape == Cirle)
            p.drawEllipse(bounds);
        else
            p.drawRect(bounds);
    }
}

inline static bool x_less_than(const QPointF &p1, const QPointF &p2) {
    return p1.x() < p2.x();
}

inline static bool y_less_than(const QPointF &p1, const QPointF &p2) {
    return p1.y() < p2.y();
}

void HoverPoints::sortPoints() {
    if (settings.sortType != SortNone) {
        QPointF currPoint;
        //Store the point at currentIndex
        if (currentIndex != -1)
            currPoint = points.at(currentIndex);

        if (settings.sortType == SortByX) {
            qStableSort(points.begin(), points.end(), x_less_than);
            
            //Have the following ends
            if (settings.followingEnds && points.size() > 2) {
                QPointF point;
                if (locks.at(0) == LockToLeft) {
                    point = points.at(0);
                    point.setY(points.at(1).y());
                    points[0] = point;
                }
                if (locks.at(points.size() - 1) == LockToRight) {
                    point = points.at(points.size() - 1);
                    point.setY(points.at(points.size() - 2).y());
                    points[points.size() - 1] = point;
                }
            }
        }
        else if (settings.sortType == SortByY) {
            qStableSort(points.begin(), points.end(), y_less_than);

            //Have the following ends
            if (settings.followingEnds && points.size() > 2) {
                QPointF point;
                if (locks.at(0) == LockToTop) {
                    point = points.at(0);
                    point.setX(points.at(1).y());
                    points[0] = point;
                }

                if (locks.at(points.size() - 1) == LockToBottom) {
                    point = points.at(points.size() - 1);
                    point.setX(points.at(points.size() - 2).x());
                    points[points.size() - 1] = point;
                }
            }
        }

        //Find the point at currentIndex and update currentIndex
        if (currentIndex != -1) {
            for (int a = 0; a < points.size(); a++) {
                if (points.at(a) == currPoint) {
                    currentIndex = findUnboundedPoint(a);
                    break;
                }
            }
        }
    }
}

//! Sorts the points and emits pointsChanged(currentIndex)
void HoverPoints::firePointChange() {
    sortPoints();
    emit pointsChanged(currentIndex);
}

//! Given an index and value, change the value of the data point (isX = value is an x value)
void HoverPoints::changeDataPoint(int index, bool isX, QString * const value) {
    bool ok;
    double doubleVal = value->toDouble(&ok);
    if (ok) {
        //String formatting handling
        int decimalIndex = value->indexOf('.');
        if (decimalIndex != -1) {
            value->truncate(decimalIndex + 7);

            if (value->startsWith('.')) {
                value->prepend('0');
            }
            doubleVal = value->toDouble(&ok);
        }

        //Set value
        QPointF point = points.at(index);
        if (isX)
            point.setX(doubleVal);
        else
            point.setY(doubleVal);

        //Sove the point
        movePoint(index, getSpacePoint(point), false);

        //save the point at index for the pointsChanged() signal
        point = points.at(index);
        sortPoints();
        parent->update();
        
        //find the saved point at index
        if (point != points.at(index)) {
            for (int a = 0; a < points.size(); a++) {
                if (point == points.at(a)) {
                    index = findUnboundedPoint(a);
                    break;
                }
            }
        }
    }
    emit pointsChanged(index);
}

void HoverPoints::removePoint(int index) {
    points.remove(index);
    locks.remove(index);
    emit pointsChanged(index);
}

void HoverPoints::addSpacePoint(int index, const QPointF & spacePoint) {
    QPointF tempPoint = bound_point(spacePoint, parent->rect(), 0);
    points.insert(index, tempPoint);
    locks.insert(index, NoLock);
    sortPoints();
    emit pointsChanged(index);
}

void HoverPoints::addDataPoint(int index, const QPointF & dataPoint) {
    addSpacePoint(index, getSpacePoint(dataPoint));
}

//! Returns the space point at the index
QPointF HoverPoints::spacePointAt(int index) const {
    return getSpacePoint(points.at(index));
}

//! Given a dataPoint, return the corresponding space point
QPointF HoverPoints::getSpacePoint(QPointF const & dataPoint) const {
    QRectF const & dataSpace = settings.dataSpace;

    if (dataSpace.isNull())
        return dataPoint;

    QPointF spacePoint;
    spacePoint.setX((dataPoint.x() - dataSpace.left())*(parent->size().width()/(double)(dataSpace.right() - dataSpace.left())));
    spacePoint.setY((dataPoint.y() - dataSpace.top())*(parent->size().height()/(double)(dataSpace.bottom() - dataSpace.top())));
    return spacePoint;
}

//! Give a space point, return the corresponding data point
QPointF HoverPoints::getDataPoint(QPointF const & spacePoint) const {
    QRectF const & dataSpace = settings.dataSpace;

    if (dataSpace.isNull())
        return spacePoint;
    
    QPointF dataPoint;
    dataPoint.setX(spacePoint.x() * ((double)(dataSpace.right() - dataSpace.left())/parent->size().width()) + dataSpace.left());
    dataPoint.setY(spacePoint.y() * ((double)(dataSpace.bottom() - dataSpace.top())/parent->size().height()) + dataSpace.top());
    return dataPoint;
}

//! Changes the data space, scaling the visual space and removing points outside of the data space
void HoverPoints::setDataSpace(const QRectF & dataSpace) {
    settings.dataSpace = dataSpace;

    //Remove or move points
    for (int a = 0; a < points.size(); a++) {
        if (!settings.dataSpace.contains(points.at(a).toPoint()) && locks.at(a) == 0) {
           points.remove(a);
           locks.remove(a);
           a--;
        }
        else {
            movePoint(a, getSpacePoint(points.at(a)), false);
        }
    }
    sortPoints();
    emit pointsChanged(-1);
    emit dataSpaceChanged(settings.dataSpace);
}

//! Loads the given profile
void HoverPoints::loadProfile(const Profile & profile) {
    setDataSpace(profile.dataSpace);
    setDataPoints(profile.points);

    //Load the locks
    for (int a = 0; a < profile.locks.size(); a++) {
        uint currLock = profile.locks.at(a);
        if (currLock != 0)
            setPointLock(a, (LockType)currLock);
    }
    emit pointsChanged();
}

//! Provides a copy of the instance's profile
void HoverPoints::getProfile(Profile & profile) {
    profile.points = QPolygonF(points);
    profile.locks = QVector<LockType>(locks);
    profile.dataSpace = settings.dataSpace;
}
