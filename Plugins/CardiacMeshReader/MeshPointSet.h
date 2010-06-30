#ifndef MESHPOINTSET_H
#define MESHPOINTSET_H

#include <QMultiMap>
#include <QList>

#include <vtkKochanekSpline.h>

class MeshPointSet
{
public:
    MeshPointSet();
    ~MeshPointSet();

    enum PointType {
      PT_ICONTOUR,
      PT_OCONTOUR,
    };

    void addPointToSet(PointType pt, int slice, int ptNum, double x, double y);

    int getMaxPtNum() { return m_maxPtNum; }
    int getMaxSlice() { return m_maxSlice; }

    double getInterpolateXValue(PointType pt, double slice, int ptNum);
    double getInterpolateYValue(PointType pt, double slice, int ptNum);

    void setContinuity(double cont);
    double getContinuity();

protected:
  void checkForSpline(QMap<int, vtkKochanekSpline*>* map, int ptNum);
  void updateSplineContinuity(QMap<int, vtkKochanekSpline*>* map);

  //! A list of all the point numbers used.
  QList<int> m_pointNumberList;

  // Mapped by point number
  QMap<int, vtkKochanekSpline*> m_iSplineMap[2];
  QMap<int, vtkKochanekSpline*> m_oSplineMap[2];

  int m_maxPtNum;
  int m_maxSlice;

  double m_continuity;
};

#endif // MESHPOINTSET_H
