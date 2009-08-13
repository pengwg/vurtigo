#ifndef RT_2D_POINT_DATA_OBJECT_H
#define RT_2D_POINT_DATA_OBJECT_H

#include "rtDataObject.h"

//! Object that represents a set of 2D points
/*!
  A data representation of a point in 2D space.
  */
class rt2DPointDataObject : public rtDataObject
{
Q_OBJECT

public:
  //! A representation of a 2D point.
  class SimplePoint2D{
    public:
      int x,y;

    bool operator==(const SimplePoint2D &other) const {
      if (x==other.x && y==other.y)
        return true;
      else
        return false;
    }

    SimplePoint2D(const SimplePoint2D& sp) {
      x = sp.x;
      y = sp.y;
    }

    SimplePoint2D& operator=(const SimplePoint2D& sp) {
      if (this == &sp)      // Same object?
        return *this;
      x = sp.x;
      y = sp.y;
    }
  };

  rt2DPointDataObject();
  ~rt2DPointDataObject();

  QList<SimplePoint2D>* getPointList() { return &m_pointList; }
  void addPoint(SimplePoint2D sp);
  void removePoint(SimplePoint2D sp);

  void update();

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  QList<SimplePoint2D> m_pointList;
};

#endif 
