#include "MeshPointSet.h"

MeshPointSet::MeshPointSet() {
  m_maxPtNum = 0;
  m_maxSlice = 0;

  m_continuity = 0.0f;

  m_iSplineMap[0].clear();
  m_oSplineMap[0].clear();
  m_iSplineMap[1].clear();
  m_oSplineMap[1].clear();

  m_pointNumberList.clear();
}

MeshPointSet::~MeshPointSet() {
  for (int coord=0; coord<2; coord++) {
    for (int ix1=0; ix1<m_pointNumberList.count(); ix1++) {
      int ptNum = m_pointNumberList[ix1];
      if(m_iSplineMap[coord].contains(ptNum)) m_iSplineMap[coord].value(ptNum)->Delete();
      if(m_oSplineMap[coord].contains(ptNum)) m_oSplineMap[coord].value(ptNum)->Delete();
    }
    m_iSplineMap[coord].clear();
    m_oSplineMap[coord].clear();
  }

  m_pointNumberList.clear();
}

void MeshPointSet::addPointToSet(MeshPointSet::PointType pt, int slice, int ptNum, double x, double y) {
  if (ptNum > m_maxPtNum) m_maxPtNum = ptNum;
  if (slice > m_maxSlice) m_maxSlice = slice;

  // Add the point numbers to the list.
  if (!m_pointNumberList.contains(ptNum)) m_pointNumberList.append(ptNum);

  if (pt == PT_ICONTOUR) {

    checkForSpline( &(m_iSplineMap[0]), ptNum);
    checkForSpline( &(m_iSplineMap[1]), ptNum);

    m_iSplineMap[0].value(ptNum)->AddPoint(slice, x);
    m_iSplineMap[1].value(ptNum)->AddPoint(slice, y);
    if (m_iSplineMap[0].value(ptNum)->GetNumberOfPoints() >= 2) {
      m_iSplineMap[0].value(ptNum)->Compute();
    }
    if (m_iSplineMap[1].value(ptNum)->GetNumberOfPoints() >= 2) {
      m_iSplineMap[1].value(ptNum)->Compute();
    }

  } else if (pt == PT_OCONTOUR){

    checkForSpline( &(m_oSplineMap[0]), ptNum);
    checkForSpline( &(m_oSplineMap[1]), ptNum);

    m_oSplineMap[0].value(ptNum)->AddPoint(slice, x);
    m_oSplineMap[1].value(ptNum)->AddPoint(slice, y);
    if (m_oSplineMap[0].value(ptNum)->GetNumberOfPoints() >= 2) {
      m_oSplineMap[0].value(ptNum)->Compute();
    }
    if (m_oSplineMap[1].value(ptNum)->GetNumberOfPoints() >= 2) {
      m_oSplineMap[1].value(ptNum)->Compute();
    }
  } else {
    // Ignore this for now...
  }

}


double MeshPointSet::getInterpolateXValue(PointType pt, double slice, int ptNum) {
  double res = 0.0;

  if (pt == PT_ICONTOUR) {
    res = m_iSplineMap[0].value(ptNum)->Evaluate(slice);
  } else if (pt == PT_OCONTOUR){
    res = m_oSplineMap[0].value(ptNum)->Evaluate(slice);
  }

  return res;
}

double MeshPointSet::getInterpolateYValue(PointType pt, double slice, int ptNum) {
  double res = 0.0;

  if (pt == PT_ICONTOUR) {
    res = m_iSplineMap[1].value(ptNum)->Evaluate(slice);
  } else if (pt == PT_OCONTOUR){
    res = m_oSplineMap[1].value(ptNum)->Evaluate(slice);
  }

  return res;
}

void MeshPointSet::setContinuity(double cont) {
  if (cont < -1.0) {
    m_continuity = -1.0;
  } else if (cont > 1.0) {
    m_continuity = 1.0;
  } else {
    m_continuity = cont;
  }
  updateSplineContinuity(&(m_iSplineMap[0]));
  updateSplineContinuity(&(m_iSplineMap[1]));
  updateSplineContinuity(&(m_oSplineMap[0]));
  updateSplineContinuity(&(m_oSplineMap[1]));
}

double MeshPointSet::getContinuity() {
  return m_continuity;
}

//////////////
// Protected Functions
///////////////
void MeshPointSet::checkForSpline(QMap<int, vtkKochanekSpline*>* map, int ptNum) {
  if (!map->contains(ptNum)) {
    vtkKochanekSpline* temp = vtkKochanekSpline::New();
    map->insert(ptNum, temp);
    temp->SetDefaultContinuity(m_continuity);
    temp->SetDefaultBias(0.0);
    temp->SetDefaultTension(1.0);
  }
}

void MeshPointSet::updateSplineContinuity(QMap<int, vtkKochanekSpline*>* map) {
  for (int coord=0; coord<2; coord++) {
    for (int ix1=0; ix1<m_pointNumberList.count(); ix1++) {
      int ptNum = m_pointNumberList[ix1];
      if(m_iSplineMap[coord].contains(ptNum)) m_iSplineMap[coord].value(ptNum)->SetDefaultContinuity(m_continuity);
      if(m_oSplineMap[coord].contains(ptNum)) m_oSplineMap[coord].value(ptNum)->SetDefaultContinuity(m_continuity);
    }
  }
}
