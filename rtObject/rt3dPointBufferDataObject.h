#ifndef RT_3D_POINT_BUFFER_DATA_OBJECT_H
#define RT_3D_POINT_BUFFER_DATA_OBJECT_H

#include "rtDataObject.h"

#include <QList>
#include <QColor>

#include "vtkProperty.h"

//! A buffer of 3D points
class rt3DPointBufferDataObject : public rtDataObject
{
Q_OBJECT

public:

  class SimplePoint {
  public:
    double px, py, pz;
    double pSize;
    vtkProperty *pProp;

    SimplePoint() {
      pProp = vtkProperty::New();
    }

    ~SimplePoint() {
      pProp->Delete();
    }

    bool operator==(const SimplePoint &other) const {
      if (px==other.px && 
	  py==other.py && 
	  pz==other.pz && 
	  pSize == other.pSize && 
	  pProp == other.pProp) 
	return true;
      else 
	return false;
    }

    SimplePoint(const SimplePoint& sp) {
      px = sp.px;
      py = sp.py;
      pz = sp.pz;
      pSize = sp.pSize;

      pProp = vtkProperty::New();
      pProp->DeepCopy(sp.pProp);
    }

    SimplePoint& operator=(const SimplePoint& sp) {
      if (this == &sp)      // Same object?
	return *this;

      px = sp.px;
      py = sp.py;
      pz = sp.pz;
      pSize = sp.pSize;

      pProp->Delete();
      pProp = vtkProperty::New();
      pProp->DeepCopy(sp.pProp);
    }

  };

  rt3DPointBufferDataObject();
  ~rt3DPointBufferDataObject();

  QList<SimplePoint>* getPointList() { return &m_pointList; }
  void addPoint(SimplePoint sp);
  void removePoint(SimplePoint sp);

  void apply();
  void update();

 protected:
  // Properties
  QList<SimplePoint> m_pointList;

  // Functions
  void setupGUI();
  void cleanupGUI();
  
};

#endif 
